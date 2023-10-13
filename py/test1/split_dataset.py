import os
import pathlib
from typing import Any
import preprocess as pp
import zipfile
import argparse
import random
import tomli_w


def picked_count(size: int, ratio: float, max_count: int) -> int:
    assert max_count != 0
    if max_count < 0:
        return int(size * ratio)
    return min(max_count, int(size * ratio))


def export_subdataset(filename: str,
                      src: zipfile.ZipFile,
                      filelist: list[zipfile.ZipInfo],
                      metainfo: dict[str, Any]):
    dst = zipfile.ZipFile(filename, 'w', compression=zipfile.ZIP_STORED)
    for info in filelist:
        content = src.read(info)
        dst.writestr(info.filename, data=content)
    dst.close()

    config_filename = filename.removesuffix(".zip") + ".toml"
    tomli_w.dump(metainfo, open(config_filename, "wb"))


def split_dataset(source: str,
                  train_dir: str, test_dir: str, validate_dir: str,
                  test_ratio: float, test_max_count: int,
                  validate_ratio: float, validate_max_count: int):
    src_ds = pp.ZipDataset(source)
    src = src_ds.zip

    filelist = src.filelist.copy()
    random.shuffle(filelist)

    test_count = picked_count(len(filelist), test_ratio, test_max_count)
    validate_count = picked_count(len(filelist), validate_ratio, validate_max_count)

    counter: int = 0

    metainfo: dict[str, Any] = src_ds.config
    metainfo['parent'] = source
    metainfo.pop('original_images')

    metainfo['generated_images'] = test_count
    metainfo['usage'] = 'test'
    export_subdataset(os.path.join(test_dir, os.path.basename(source)),
                      src=src,
                      filelist=filelist[counter:counter + test_count],
                      metainfo=metainfo
                      )
    counter += test_count

    metainfo['generated_images'] = validate_count
    metainfo['usage'] = 'validate'
    export_subdataset(os.path.join(validate_dir, os.path.basename(source)),
                      src=src,
                      filelist=filelist[counter:counter + validate_count],
                      metainfo=metainfo
                      )
    counter += validate_count

    metainfo['generated_images'] = len(src.filelist) - validate_count - test_count
    metainfo['usage'] = 'train'
    export_subdataset(os.path.join(train_dir, os.path.basename(source)),
                      src=src,
                      filelist=filelist[counter:len(src.filelist)],
                      metainfo=metainfo
                      )


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("source", type=str, nargs='+')
    parser.add_argument("--out-dir", type=str)
    parser.add_argument("--test-ratio", "--tr", type=float)
    parser.add_argument("--test-max-count", "--tmc", type=int, required=False, default=-1)
    parser.add_argument("--validate-ratio", "--vr", type=float)
    parser.add_argument("--validate-max-count", "--vmc", type=int, required=False, default=-1)
    args = parser.parse_args()

    out_dir = args.out_dir
    train_dir = os.path.join(out_dir, "train")
    test_dir = os.path.join(out_dir, "test")
    validate_dir = os.path.join(out_dir, "validate")

    os.makedirs(train_dir, exist_ok=True)
    os.makedirs(test_dir, exist_ok=True)
    os.makedirs(validate_dir, exist_ok=True)

    for src in args.source:
        print(f"Splitting {src}...")
        split_dataset(src,
                      train_dir=train_dir,
                      test_dir=test_dir,
                      validate_dir=validate_dir,

                      test_ratio=args.test_ratio,
                      test_max_count=args.test_max_count,

                      validate_ratio=args.validate_ratio,
                      validate_max_count=args.validate_max_count)


if __name__ == "__main__":
    main()
