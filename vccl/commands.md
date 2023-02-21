```bash
vccl --rp D:/Git/SlopeCraft/binaries/Vanilla_1_19_2.zip --bsl D:/Git/SlopeCraft/VisualCraftL/VCL_blocks_fixed.json --layers 3 --face up --version 19 --algo RGB --prefix ./ --img D:/Git/SlopeCraft/binaries/images/test_image_01.png --benchmark -j20

run -j1 --rp D:/Git/SlopeCraft/binaries/Vanilla_1_19_2.zip --bsl D:/Git/SlopeCraft/VisualCraftL/VCL_blocks_fixed.json --layers 3 --face up --version 19 --algo RGB --prefix ./ --img D:/Git/SlopeCraft/binaries/images/test_image_01.png

vccl --rp D:/Git/SlopeCraft/binaries/Vanilla_1_19_2.zip --bsl D:/Git/SlopeCraft/VisualCraftL/VCL_blocks_fixed.json --layers 3 --face up --version 19 --prefix ./cpu_ --img D:/Git/SlopeCraft/binaries/images/test_image_01.png --benchmark -j20 --out-image --algo RGB

vccl --rp D:/Git/SlopeCraft/binaries/Vanilla_1_19_2.zip --bsl D:/Git/SlopeCraft/VisualCraftL/VCL_blocks_fixed.json --layers 3 --face up --version 19 --prefix ./gpu_ --img D:/Git/SlopeCraft/binaries/images/test_image_01.png --benchmark -j20 --out-image --algo RGB --gpu



vccl --rp D:/Git/SlopeCraft/binaries/Vanilla_1_19_2.zip --bsl D:/Git/SlopeCraft/VisualCraftL/VCL_blocks_fixed.json --layers 1 --face up --version 13 --prefix ./gpu_ --img D:/Git/SlopeCraft/binaries/images/test_image_01.png --benchmark -j20 --out-image --algo RGB --gpu
```


```bash
./vccl --rp $D/Git/SlopeCraft/binaries/Vanilla_1_19_2.zip --bsl $D/Git/SlopeCraft/VisualCraftL/VCL_blocks_fixed.json --layers 3 --face up --version 19 --prefix ./ --img $D/Git/SlopeCraft/binaries/images/test_image_01.png --benchmark -j20 --algo RGB_Better
```

