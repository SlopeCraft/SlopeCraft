import copy
import io

import cv2
import os
import pathlib
import argparse
import numpy as np
import multiprocessing as mp
import zipfile
import time
import hashlib
import tomllib
import tomli_w
from typing import Any
import torch
import torch.utils.data


def cut_image(input_img: cv2.typing.MatLike,
              row_size: int,
              col_size: int) -> list[cv2.typing.MatLike]:
    result = []
    rows = input_img.shape[0]
    cols = input_img.shape[1]
    rows = rows - rows % row_size
    cols = cols - cols % col_size

    for r in range(0, rows, row_size):
        for c in range(0, cols, col_size):
            result.append(input_img[r:r + row_size, c:c + col_size])

    return result


def process_dir(source_dir: str, archive_name: str, cutted_rows: int, cutted_cols: int) -> dict[str, Any]:
    read_counter: int = 0
    write_counter: int = 0
    duplicate_counter: int = 0
    archive = zipfile.ZipFile(archive_name, mode='w', compression=zipfile.ZIP_STORED)

    hash_set = set()

    for filepath, _, filenames in os.walk(source_dir):
        for filename in filenames:
            abs_filename = os.path.join(filepath, filename)

            if not os.path.isfile(abs_filename):
                raise RuntimeError(f"Non existing file {abs_filename}")

            if filename == ".nomedia":
                continue

            img = cv2.imdecode(np.fromfile(abs_filename, dtype=np.uint8), cv2.IMREAD_COLOR)
            if img is None:
                print(f"Failed to read {abs_filename}, skip and continue.")
                continue

            read_counter += 1

            if read_counter % 100 == 0:
                print(f"{read_counter} images read, {write_counter} images generated.")

            cut_imgs = cut_image(img, cutted_rows, cutted_cols)

            for cut_img in cut_imgs:
                core_filename = f"{write_counter:014}.jpg"
                ok, encoded = cv2.imencode(".jpg", cut_img)
                if not ok:
                    raise RuntimeError(f"Failed to encode {core_filename}")

                encoded_bytes = encoded.tobytes()
                img_hash = hashlib.sha1()
                img_hash.update(encoded_bytes)

                img_hash = img_hash.hexdigest()
                if img_hash in hash_set:
                    duplicate_counter += 1
                    continue

                hash_set.add(img_hash)
                write_counter += 1
                archive.writestr(core_filename, encoded_bytes)

        archive.close()
        print(f"{read_counter} images read, {duplicate_counter} duplicated images removed., {write_counter} images "
              f"generated")

        return {'original_images': read_counter,
                'generated_images': write_counter,
                'source_dir': source_dir,
                'rows': cutted_rows,
                'cols': cutted_cols}


def parse_shape_str(shape_str: str) -> tuple[int, int]:
    split = shape_str.split('x')
    if len(split) != 2:
        raise RuntimeError(f"Invalid shape string {shape_str}")

    result = [0, 0]
    for idx in range(0, 2):
        temp = int(split[idx])
        if temp <= 0:
            raise RuntimeError(f"Invalid shape: {temp}")
        result[idx] = temp

    return result[0], result[1]


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument("--source-dir", type=str, required=True, nargs='+')
    parser.add_argument("--out-dir", type=str, default="preprocessed")
    parser.add_argument("--shape", type=str, default="128x128", nargs='+')

    args = vars(parser.parse_args())

    out_dir = args["out_dir"]
    os.makedirs(out_dir, exist_ok=True)

    source_dirs: list[str] = args["source_dir"]

    shapes = []
    for shape_str in args["shape"]:
        shapes.append(parse_shape_str(shape_str))

    # metainfos = dict()

    for src_dir_name in source_dirs:
        print(f"Processing images in {src_dir_name}...")
        for shape in shapes:
            print(f"Cutting with shape str = {shape}...")
            cut_rows = shape[1]
            cut_cols = shape[0]

            filename_stem = f"{pathlib.Path(src_dir_name).stem}-{cut_cols}x{cut_rows}"

            archive_name = os.path.join(out_dir, f"{filename_stem}.zip")
            abs_time = time.time()
            info = process_dir(src_dir_name, archive_name, cut_rows, cut_cols)
            seconds = time.time() - abs_time
            print(f"Finished in {seconds} seconds")

            tomli_w.dump(info, open(os.path.join(out_dir, f"{filename_stem}.toml"), "wb"))

            # metainfos[archive_name] = info

    # tomli_w.dump(metainfos, open(os.path.join(out_dir, "total_info.toml"), 'wb'))


class ZipDataset(torch.utils.data.Dataset):
    config: dict[str, Any]
    zip: zipfile.ZipFile

    def __init__(self, zip_file: str):
        config_file = zip_file.removesuffix(".zip") + ".toml"
        if not os.path.isfile(config_file):
            raise RuntimeError(f"Expected {config_file} as the config file for {zip_file}, but doesn't exist.")

        self.config = tomllib.load(open(config_file, "rb"))
        print(f"Loading {zip_file}...")
        self.zip = zipfile.ZipFile(zip_file, mode='r')

    def __len__(self) -> int:
        return len(self.zip.filelist)

    @staticmethod
    def read_image_binary(zip_info: zipfile.ZipInfo) -> bytes:
        return zip_obj.read(name=zip_info)

    def decode_image(self, zip_info: zipfile.ZipInfo) -> torch.Tensor:
        encoded = self.read_image_binary(zip_info)
        encoded_nparr = np.frombuffer(encoded, dtype=np.uint8)
        image = cv2.imdecode(encoded_nparr, flags=cv2.IMREAD_COLOR)
        image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

        image = np.array(image, dtype=np.float32) / 255

        tensor = torch.from_numpy(image)
        tensor = torch.permute(tensor, dims=[2, 0, 1])
        return tensor

    def __getitem__(self, item) -> torch.Tensor:
        return self.decode_image(self.zip.filelist[item])

    # def __iter__(self):
    #     for zip_info in self.zip.filelist:
    #         tensor=self.decode_image(zip_info)
    #         yield tensor


class MPZipDataset(ZipDataset):
    major_pid: int
    zip_dict: dict[int, zipfile.ZipFile]

    def __init__(self, zip_file: str, major_pid: int = os.getpid()):
        super().__init__(zip_file)
        self.major_pid = os.getpid()
        self.zip_dict = {}

    def select_zip_obj(self) -> zipfile.ZipFile:
        pid = os.getpid()
        if pid == self.major_pid:
            return self.zip

        if pid not in self.zip_dict:
            print(f"Reloading {self.zip.filename} for process {pid}...")
            # new_zip = zipfile.ZipFile(self.zip.filename, mode='r')
            new_zip = self.zip
            new_zip.fp = io.open(self.zip.filename,self.zip.fp.mode)
            self.zip_dict[pid] = new_zip

        return self.zip_dict[pid]

    # @override
    def read_image_binary(self, zip_info: zipfile.ZipInfo) -> bytes:
        return self.select_zip_obj().read(zip_info)


if __name__ == "__main__":
    main()
