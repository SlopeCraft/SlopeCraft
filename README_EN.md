# SlopeCraft

![](https://img.shields.io/badge/C%2B%2B-11-blue?style=plastic) ![](https://img.shields.io/badge/Qt-v6.0.4-brightgreen?style=plastic) ![](https://img.shields.io/badge/Eigen-v3.3.9-yellowgreen?style=plastic) 

![](https://img.shields.io/badge/Minecraft-1.12~1.17-orange?style=plastic) 

![GitHub tag (latest SemVer)](https://img.shields.io/github/v/tag/TokiNoBug/SlopeCraft?style=plastic) ![GitHub](https://img.shields.io/github/license/TokiNoBug/SlopeCraft?style=plastic)

<br>

[简体中文](README.md "README.md") | **English**    <!-- lang -->

<br>

> Get your 3D pixel painting in minecraft.


## Tutorials:

* [SlopeCraftv3.1 Tutorial](https://github.com/ToKiNoBug/SlopeCraftTutorial/blob/main/v3.1Tutorial/v3.1Tutorial.md)
* [SlopeCraftv3.1 使用教程](https://github.com/ToKiNoBug/SlopeCraftTutorial/blob/main/v3.1Tutorial/v3.1%E6%95%99%E7%A8%8B%E4%B8%AD%E6%96%87%E7%89%88.md)
* [SlopeCraft傻瓜级使用教程](https://github.com/ToKiNoBug/SlopeCraftTutorial/blob/main/v3.0Tutorial/%E5%82%BB%E7%93%9C%E7%BA%A7%E4%BD%BF%E7%94%A8%E6%95%99%E7%A8%8B.md)

## FAQ：

[SlopeCraft FAQ](./FAQ.md)


## Program Description

SlopeCraft is created by me(TokiNoBug) using qt+Eigen+zlib - it's used to create 3D pixel map arts within Minecraft.

The difference between this program and SpriteCraft is that this program focuses on pixel art on maps. Its purpose is to record the pixel art with the map, then display it in an item frame (the map here refers to the in-game item, and not the save file).

The color modification module is targeted towards the map. The pixel art is meant to resemble the art within a map - it will not resemble the art from the point of view of the player.

Since the color within the map is related to the relative height of the block, the pixel art created by SlopeCraft is usually in 3D, which I call 3D Pixel Art.

In summary, SlopeCraft is created for map pixel art.

This is why maps made by SlopeCraft have higher quality than exporting a picture from SpriteCraft then recording it with a map - because SlopeCraft is specifically designed for map pixel art.


## Game Version, Type, Color Space and Block List

### 1.Game Version: SlopeCraft supports 1.12 to 1.16. There isn't much difference from 1.12 to 1.15, but the addition of nether trees added 28 more colors, creating a total of 232 colors (ignoring transparent blocks).

### 2. Types of Map Art:

File-only Map Art uses the full color palette. It has the highest quality, but it can't be exported into the world itself. It can only be exported as a map file, using commands to obtain it in game. 


3D Map Art leaves 1/4th of the colors out as a trade off for being able to export it into vanilla survival. This type of map art can generate 3D map arts, which is the core feature of this program.

Flat Map Art is the traditional 2D map art - it's the easiest to build but it only have 1/4th of the color palette compared to File-only: 51/58/60 colors.


### 3. Color Space:

This is more difficult to explain. Simply put, the program describes the color in different ways in each color space. In SlopeCraft, this means different ways of choosing a block for a color

The four types of color space: RGB, HSV, LAB, XYZ

Lab is the best quality for most pictures, which is why it is default, but the others should still be tried to see which color space has the best visual effect for your picture.

### 4. Block List:

This is the most important part, it controls how the final product looks. Simply put, every color within a picture has multiple blocks with its color, but we can only choose one when we export it, which means only one block of each color can be chosen

So the Block List is more like the Material List of the pixel art. 4 Presets have been set: Vanilla, Cheap, Elegant and Shiny.

Vanilla block lists try to recreate the color of the original picture as much as possible, such as the color of sand being sandstone, the color of diamond blocks will use diamond blocks. This preset is usually not used within survival (have fun trying to get 3000 Diamond Blocks), but for creative mode purposes, which is why I call it Creative.

Cheap is a highly optimized block list - every color will use its most easily obtainable block counterpart. The color red would use netherrack, the color white would use snow. This is best for survival mode when resources aren't abundant. 

Elegant: The most "Elegant" Block List, it mostly uses blocks that can be mass produced in survival while still looking clean, such as white being white concrete. It's best for late stages of survival, when resources are abundant.

Shiny is created specifically for use with shaders, so it mostly uses blocks that emits light.

Custom is for you to freely choose the blocks you would like to use.


## Features of SlopeCraft
These features are included in SlopeCraft

Importing Pictures, Adjusting Colors, Generating 3D map art, exporting into a file for Minecraft


## Installation

No Installation Required

## Usage Guide

1. Find the picture you would like to turn into a pixel art. Use Photoshop or other photo editing softwares to make sure the length and width of the picture in pixels are multiples of 128, which is best for maps in Minecraft.
2. Open SlopeCraft.exe, and choose the type of map art you would like to create in the start menu.
3. Click the "Import Photo" button, and choose the modified photo from step 1. The program will then import it.
4. Choose the right game version for your map art, click Confirm, and click Next Page.
5. Choose the block list for your map art - usually you can skip this step and just choose Confirm.
6. In the color adjusting page, choose any color space, choose Convert to Map, then click one of the export options.
7. In the Export page (different for each export option), follow the instructions in the page.
