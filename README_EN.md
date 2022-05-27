# SlopeCraft

![](https://img.shields.io/badge/C%2B%2B-17-blue?style=plastic) ![](https://img.shields.io/badge/Qt-v6.1.0-brightgreen?style=plastic) ![](https://img.shields.io/badge/Eigen-v3.4.0-yellowgreen?style=plastic) 

![](https://img.shields.io/badge/Minecraft-1.12~1.18-orange?style=plastic) 

![GitHub tag (latest SemVer)](https://img.shields.io/github/v/tag/TokiNoBug/SlopeCraft?style=plastic) ![GitHub](https://img.shields.io/github/license/TokiNoBug/SlopeCraft?style=plastic)

<br>

[简体中文](README.md "README.md") | **English**    <!-- lang -->

<br>

> Get your Minecraft pixel painting in multiple kinds of forms!

<br>

# Principles

* [Pinciple of Map Pixel Arts](https://github.com/ToKiNoBug/SlopeCraftTutorial/blob/main/en_US/BasicPrinciple/README.md)

# Builds for Other Platforms

* [SlopeCraft for macOS](https://github.com/iXORTech/SlopeCraft-for-macOS) - Compatibility changes and builds for macOS

# Tutorials

* [SlopeCraftv3.1 Tutorial](https://github.com/ToKiNoBug/SlopeCraftTutorial/tree/v3.7/en_US/v3.1)
* [SlopeCraft v3.6 UserGuide](https://github.com/ToKiNoBug/SlopeCraftTutorial/blob/v3.6/en/v3.6/v3.6Tutorial.tex)
* [SlopeCraft v3.7 UserGuide](https://github.com/ToKiNoBug/SlopeCraftTutorial/blob/v3.7/zh_CN/v3.7/v3.7教程.tex)

# FAQ

[SlopeCraft FAQ](./docs/FAQ_EN.md)

# Program Description

SlopeCraft is created using Qt + Eigen + zlib for creating 3D pixel map arts within Minecraft.

The difference between this program and SpriteCraft is that this program focuses on pixel art on maps. Its purpose is to record the pixel art with the map, then display it in an item frame (the map here refers to the in-game item, and not the saving files).

The color modification module is targeted towards the map. The pixel art is meant to resemble the art in the "point of view" of the map - not of the player.

Since the color within the map is related to the relative height of the block, the pixel art created by SlopeCraft is usually in 3D, which I call 3D Pixel Art.

In summary, SlopeCraft is created for map pixel art.

This is why maps made by SlopeCraft have higher quality than exporting a picture from SpriteCraft then recording it with a map - because SlopeCraft is specifically designed for map pixel art.

## Game Version, Type, Converting Algorithm and Block List

### 1. Game Versions

SlopeCraft supports Minecraft Java Edition 1.12 to 1.16. There isn't much difference from 1.12 to 1.15, but the addition of nether trees added 28 more colors, creating a total of 232 colors, and 12 more colors added in 1.17, creating a total of 244 colors (ignoring transparent blocks).

### 2. Types of Map Art

File-only Map Art uses the full color palette. It has the highest quality, but it can't be exported into the world itself. It can only be exported as a map file, using commands to obtain it in game.

3D Map Art leaves 1/4th of the colors out as a trade off for being able to export it into vanilla survival. This type of map art can generate 3D map arts, which is the core feature of this program.

Flat Map Art is the traditional 2D map art - it's the easiest to build but it only have 1/4th of the color palette compared to File-only: 51/58/61 colors.

### 3. Converting Algorithm

This is more difficult to explain. Simply put, your map art should only consist of map colors. an converting algorithm is a method to find a best-matched map color for each image color. In fact, the essence of a converting algorithm is a color difference formula.

The 6 converting algorithm: RGB, RGB+, HSV, Lab94, Lab00 and XYZ. Besides, dithering can be enabled to trade color with resolution by mixing similiar colors.

RGB+ is the best quality for most images, which is why it is default, but the others should still be tried to see which color space has the best visual effect for your image.

### 4. Block List

This is the most important part, it controls how the final product looks. Simply put, every color within a picture has multiple blocks with its color, but we can only choose one when we export it, which means only one block of each color can be chosen

So the Block List is more like the Material List of the pixel art. 4 Presets have been set: Vanilla, Cheap, Elegant and Shiny.

Vanilla block lists try to recreate the color of the original picture as much as possible, such as the color of sand being sandstone, the color of diamond blocks will use diamond blocks. This preset is usually not used within survival (have fun trying to get 3000 Diamond Blocks), but for creative mode purposes, which is why I call it Creative.

Cheap is a highly optimized block list - every color will use its most easily obtainable block counterpart. The color red would use netherrack, the color white would use snow. This is best for survival mode when resources aren't abundant.

Elegant: The most "Elegant" Block List, it mostly uses blocks that can be mass produced in survival while still looking clean, such as white being white concrete. It's best for late stages of survival, when resources are abundant.

Shiny is created specifically for use with shaders, so it mostly uses blocks that emits light.

Custom is for you to freely choose the blocks you would like to use.

## Features of SlopeCraft

These features are included in SlopeCraft

Importing image, Converting image, Generating 3D/Flat/File-only map art, exporting into Schematic file/Structure file/map data file/image.

## Installation

No Installation Required

## Usage Guide

See tutorials listed above.

# Other Related Repos

* [NBTWriter](https://github.com/ToKiNoBug/NBTWriter-of-Toki) - Lib for writing NBT files.
* [SlopeCraftTutorial](https://github.com/ToKiNoBug/SlopeCraftTutorial) - Tutorials
* [SlopeCraftCompressLib](https://github.com/ToKiNoBug/SlopeCraftCompressLib) - Lib for building height map and lossless compression lib.
* [SlopeCraftLossyCompression](https://github.com/ToKiNoBug/SlopeCraftLossyCompression) - Lossy compression lib, based on SlopeCraftCompressLib.
* [SlopeCraftGlassBuilder](https://github.com/ToKiNoBug/SlopeCraftGlassBuilder) - Glass bridge building lib.
* [HeuristicFlow](https://github.com/TokiNoBug/HeuristicFlow) - GA implementation.
* [SlopeCraft for macOS](https://github.com/iXORTech/SlopeCraft-for-macOS) - Compatibility changes and builds for macOS
