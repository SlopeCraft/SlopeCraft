/*
 Copyright © 2021  TokiNoBug
This file is part of SlopeCraft.

    SlopeCraft is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SlopeCraft is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SlopeCraft.  If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/ToKiNoBug
    bilibili:https://space.bilibili.com/351429231
*/


#include "mainwindow.h"
#include "ui_mainwindow.h"

#include  <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QString>
#ifndef Collecter_H
#define Collecter_H

void MainWindow::Collect()
{
    for(short Base=0;Base<64;Base++)
    {Enables[Base]=NULL;
        ShowColors[Base]=NULL;
        for(short j=0;j<12;j++)
        {
            Blocks[Base][j]=NULL;
            Data.FullBlockList[Base][j].id="DefaultBlockId";
            Data.FullBlockList[Base][j].idOld="[]";
            Data.FullBlockList[Base][j].version=0;//0代表很早以前就出现的方块，255代表没有对应的方块
        }

    }
    //////////////
    Enables[0]=ui->Enable00;
    Enables[1]=ui->Enable01;
    Enables[2]=ui->Enable02;
    Enables[3]=ui->Enable03;
    Enables[4]=ui->Enable04;
    Enables[5]=ui->Enable05;
    Enables[6]=ui->Enable06;
    Enables[7]=ui->Enable07;
    Enables[8]=ui->Enable08;
    Enables[9]=ui->Enable09;
    Enables[10]=ui->Enable10;
    Enables[11]=ui->Enable11;
    Enables[12]=ui->Enable12;
    Enables[13]=ui->Enable13;
    Enables[14]=ui->Enable14;
    Enables[15]=ui->Enable15;
    Enables[16]=ui->Enable16;
    Enables[17]=ui->Enable17;
    Enables[18]=ui->Enable18;
    Enables[19]=ui->Enable19;
    Enables[20]=ui->Enable20;
    Enables[21]=ui->Enable21;
    Enables[22]=ui->Enable22;
    Enables[23]=ui->Enable23;
    Enables[24]=ui->Enable24;
    Enables[25]=ui->Enable25;
    Enables[26]=ui->Enable26;
    Enables[27]=ui->Enable27;
    Enables[28]=ui->Enable28;
    Enables[29]=ui->Enable29;
    Enables[30]=ui->Enable30;
    Enables[31]=ui->Enable31;
    Enables[32]=ui->Enable32;
    Enables[33]=ui->Enable33;
    Enables[34]=ui->Enable34;
    Enables[35]=ui->Enable35;
    Enables[36]=ui->Enable36;
    Enables[37]=ui->Enable37;
    Enables[38]=ui->Enable38;
    Enables[39]=ui->Enable39;
    Enables[40]=ui->Enable40;
    Enables[41]=ui->Enable41;
    Enables[42]=ui->Enable42;
    Enables[43]=ui->Enable43;
    Enables[44]=ui->Enable44;
    Enables[45]=ui->Enable45;
    Enables[46]=ui->Enable46;
    Enables[47]=ui->Enable47;
    Enables[48]=ui->Enable48;
    Enables[49]=ui->Enable49;
    Enables[50]=ui->Enable50;
    Enables[51]=ui->Enable51;
    Enables[52]=ui->Enable52;
    Enables[53]=ui->Enable53;
    Enables[54]=ui->Enable54;
    Enables[55]=ui->Enable55;
    Enables[56]=ui->Enable56;
    Enables[57]=ui->Enable57;
    Enables[58]=ui->Enable58;
    Enables[59]=ui->Enable59;
    Enables[60]=ui->Enable60;
    /////////////////
    ShowColors[0]=ui->Color00;
    ShowColors[1]=ui->Color01;
    ShowColors[2]=ui->Color02;
    ShowColors[3]=ui->Color03;
    ShowColors[4]=ui->Color04;
    ShowColors[5]=ui->Color05;
    ShowColors[6]=ui->Color06;
    ShowColors[7]=ui->Color07;
    ShowColors[8]=ui->Color08;
    ShowColors[9]=ui->Color09;
    ShowColors[10]=ui->Color10;
    ShowColors[11]=ui->Color11;
    ShowColors[12]=ui->Color12;
    ShowColors[13]=ui->Color13;
    ShowColors[14]=ui->Color14;
    ShowColors[15]=ui->Color15;
    ShowColors[16]=ui->Color16;
    ShowColors[17]=ui->Color17;
    ShowColors[18]=ui->Color18;
    ShowColors[19]=ui->Color19;
    ShowColors[20]=ui->Color20;
    ShowColors[21]=ui->Color21;
    ShowColors[22]=ui->Color22;
    ShowColors[23]=ui->Color23;
    ShowColors[24]=ui->Color24;
    ShowColors[25]=ui->Color25;
    ShowColors[26]=ui->Color26;
    ShowColors[27]=ui->Color27;
    ShowColors[28]=ui->Color28;
    ShowColors[29]=ui->Color29;
    ShowColors[30]=ui->Color30;
    ShowColors[31]=ui->Color31;
    ShowColors[32]=ui->Color32;
    ShowColors[33]=ui->Color33;
    ShowColors[34]=ui->Color34;
    ShowColors[35]=ui->Color35;
    ShowColors[36]=ui->Color36;
    ShowColors[37]=ui->Color37;
    ShowColors[38]=ui->Color38;
    ShowColors[39]=ui->Color39;
    ShowColors[40]=ui->Color40;
    ShowColors[41]=ui->Color41;
    ShowColors[42]=ui->Color42;
    ShowColors[43]=ui->Color43;
    ShowColors[44]=ui->Color44;
    ShowColors[45]=ui->Color45;
    ShowColors[46]=ui->Color46;
    ShowColors[47]=ui->Color47;
    ShowColors[48]=ui->Color48;
    ShowColors[49]=ui->Color49;
    ShowColors[50]=ui->Color50;
    ShowColors[51]=ui->Color51;
    ShowColors[52]=ui->Color52;
    ShowColors[53]=ui->Color53;
    ShowColors[54]=ui->Color54;
    ShowColors[55]=ui->Color55;
    ShowColors[56]=ui->Color56;
    ShowColors[57]=ui->Color57;
    ShowColors[58]=ui->Color58;
    ShowColors[59]=ui->Color59;
    ShowColors[60]=ui->Color60;
    ////////////////
    Blocks[0][0]=ui->is00Glass;     Data.FullBlockList[0][0].id="glass";

    Blocks[1][0]=ui->is01Grass;     Data.FullBlockList[1][0].id="grass_block[snowy=false]";      Data.FullBlockList[1][0].idOld="grass[snowy=false]";
    Blocks[1][1]=ui->is01SlimeBlock;        Data.FullBlockList[1][1].id="slime_block";       Data.FullBlockList[1][1].idOld="slime";

    Blocks[2][0]=ui->is02BirchPlanks;       Data.FullBlockList[2][0].id="birch_planks";      Data.FullBlockList[2][0].idOld="planks[variant=birch]";
    Blocks[2][1]=ui->is02SmoothSandstone;       Data.FullBlockList[2][1].id="smooth_sandstone";      Data.FullBlockList[2][1].idOld="sandstone[type=smooth_sandstone]";
    Blocks[2][2]=ui->is02Glowstone;       Data.FullBlockList[2][2].id="glowstone";
    Blocks[2][3]=ui->is02EndStone;       Data.FullBlockList[2][3].id="end_stone";
    Blocks[2][4]=ui->is02EndBricks;       Data.FullBlockList[2][4].id="end_stone_bricks";
    Blocks[2][5]=ui->is02TurtleEgg;       Data.FullBlockList[2][5].id="turtle_egg[eggs=4,hatch=0]";Data.FullBlockList[2][5].version=13;

    Blocks[3][0]=ui->is03MushroomStem;       Data.FullBlockList[3][0].id="mushroom_stem[east=true,west=true,north=true,south=true,up=true,down=true]";Data.FullBlockList[3][0].version=13;
    Blocks[3][1]=ui->is03CobWeb;       Data.FullBlockList[3][1].id="cobweb";       Data.FullBlockList[3][1].idOld="web";

    Blocks[4][0]=ui->is04RedstoneBlock;       Data.FullBlockList[4][0].id="redstone_block";
    Blocks[4][1]=ui->is04TNT;       Data.FullBlockList[4][1].id="tnt[unstable=false]";       Data.FullBlockList[4][1].idOld="tnt[explode=false]";

    Blocks[5][0]=ui->is05Ice;       Data.FullBlockList[5][0].id="ice";
    Blocks[5][1]=ui->is05PackedIce;       Data.FullBlockList[5][1].id="packed_ice";
    Blocks[5][2]=ui->is05BlueIce;       Data.FullBlockList[5][2].id="blue_ice";Data.FullBlockList[5][2].version=13;

    Blocks[6][0]=ui->is06IronBlock;       Data.FullBlockList[6][0].id="iron_block";
    Blocks[6][1]=ui->is06BrewingStand;       Data.FullBlockList[6][1].id="brewing_stand[has_bottle_0=false,has_bottle_1=false,has_bottle_2=false]";
    Blocks[6][2]=ui->is06IronPlate;       Data.FullBlockList[6][2].id="heavy_weighted_pressure_plate[power=0]";
    Blocks[6][3]=ui->is06IronTrapDoor;       Data.FullBlockList[6][3].id="iron_trapdoor[facing=north,half=top,open=false,powered=false,waterlogged=false]";      Data.FullBlockList[6][3].idOld="iron_trapdoor[facing=north,half=top,open=false]";
    Blocks[6][4]=ui->is06Lantern;       Data.FullBlockList[6][4].id="lantern[hanging=false]";Data.FullBlockList[6][4].version=14;
    Blocks[6][5]=ui->is06Grindstone;       Data.FullBlockList[6][5].id="grindstone[face=floor,facing=north]";Data.FullBlockList[6][5].version=14;

    Blocks[7][0]=ui->is07OakLeaves;       Data.FullBlockList[7][0].id="oak_leaves[distance=7,persistent=true]";      Data.FullBlockList[7][0].idOld="leaves[variant=oak,check_decay=false,decayable=false]";
    Blocks[7][1]=ui->is07SpruceLeaves;       Data.FullBlockList[7][1].id="spruce_leaves[distance=7,persistent=true]";      Data.FullBlockList[7][1].idOld="leaves[variant=spruce,check_decay=false,decayable=false]";
    Blocks[7][2]=ui->is07BirchLeaves;       Data.FullBlockList[7][2].id="birch_leaves[distance=7,persistent=true]";      Data.FullBlockList[7][2].idOld="leaves[variant=birch,check_decay=false,decayable=false]";
    Blocks[7][3]=ui->is07JungleLeaves;       Data.FullBlockList[7][3].id="jungle_leaves[distance=7,persistent=true]";      Data.FullBlockList[7][3].idOld="leaves[variant=jungle,check_decay=false,decayable=false]";
    Blocks[7][4]=ui->is07AcaciaLeaves;       Data.FullBlockList[7][4].id="acacia_leaves[distance=7,persistent=true]";      Data.FullBlockList[7][4].idOld="leaves2[variant=acacia,check_decay=false,decayable=false]";
    Blocks[7][5]=ui->is07DarkOakLeaves;       Data.FullBlockList[7][5].id="dark_oak_leaves[distance=7,persistent=true]";      Data.FullBlockList[7][5].idOld="leaves2[variant=dark_oak,check_decay=false,decayable=false]";

    Blocks[8][0]=ui->is08Concrete;       Data.FullBlockList[8][0].id="white_concrete";     Data.FullBlockList[8][0].idOld="concrete[color=white]";
    Blocks[8][1]=ui->is08Wool;       Data.FullBlockList[8][1].id="white_wool";     Data.FullBlockList[8][1].idOld="wool[color=white]";
    Blocks[8][2]=ui->is08StainedGlass;       Data.FullBlockList[8][2].id="white_stained_glass";     Data.FullBlockList[8][2].idOld="stained_glass[color=white]";
    Blocks[8][3]=ui->is08SnowBlock;       Data.FullBlockList[8][3].id="snow_block";      Data.FullBlockList[8][3].idOld="snow";

    Blocks[9][0]=ui->is09Clay;      Data.FullBlockList[9][0].id="clay";

    Blocks[10][0]=ui->is10CoarseDirt;      Data.FullBlockList[10][0].id="coarse_dirt";Data.FullBlockList[10][0].version=13;       Data.FullBlockList[10][0].idOld="dirt[variant=coarse_dirt,snowy=false]";
    Blocks[10][1]=ui->is10PolishedGranite;      Data.FullBlockList[10][1].id="polished_granite";     Data.FullBlockList[10][1].idOld="stone[variant=smooth_granite]";
    Blocks[10][2]=ui->is10JunglePlanks;      Data.FullBlockList[10][2].id="jungle_planks";      Data.FullBlockList[10][2].idOld="planks[variant=jungle]";
    Blocks[10][3]=ui->is10BrownMushroomBlock;      Data.FullBlockList[10][3].id="brown_mushroom_block[east=true,west=true,north=true,south=true,up=true,down=true]";     Data.FullBlockList[10][3].idOld="brown_mushroom_block[variant=all_outside]";
    Blocks[10][4]=ui->is10Dirt;      Data.FullBlockList[10][4].id="dirt";        Data.FullBlockList[10][4].idOld="dirt[variant=dirt,snowy=false]";

    Blocks[11][0]=ui->is11Cobblestone;      Data.FullBlockList[11][0].id="cobblestone";
    Blocks[11][1]=ui->is11Stone;      Data.FullBlockList[11][1].id="stone";      Data.FullBlockList[11][1].idOld="stone[variant=stone]";
    Blocks[11][2]=ui->is11SmoothStone;      Data.FullBlockList[11][2].id="smooth_stone";     Data.FullBlockList[11][2].version=13;
    Blocks[11][3]=ui->is11StoneBricks;      Data.FullBlockList[11][3].id="stone_bricks";Data.FullBlockList[11][3].idOld="stonebrick[variant=stonebrick]";

    Blocks[12][0]=ui->is12Water;      Data.FullBlockList[12][0].id="water[level=0]";

    Blocks[13][0]=ui->is13OakPlanks;      Data.FullBlockList[13][0].id="oak_planks";     Data.FullBlockList[13][0].idOld="planks[variant=oak]";

    Blocks[14][0]=ui->is14PolishedDiorite;      Data.FullBlockList[14][0].id="polished_diorite";     Data.FullBlockList[14][0].idOld="stone[variant=smooth_diorite]";
    Blocks[14][1]=ui->is14QuartzBlock;      Data.FullBlockList[14][1].id="quartz_block";     Data.FullBlockList[14][1].idOld="quartz_block[variant=default]";
    Blocks[14][2]=ui->is14SeaLantern;      Data.FullBlockList[14][2].id="sea_lantern";
    Blocks[14][3]=ui->is14Target;      Data.FullBlockList[14][3].id="target[power=0]";Data.FullBlockList[14][3].version=16;

    Blocks[15][0]=ui->is15Concrete;      Data.FullBlockList[15][0].id="orange_concrete";     Data.FullBlockList[15][0].idOld="concrete[color=orange]";
    Blocks[15][1]=ui->is15Wool;      Data.FullBlockList[15][1].id="orange_wool";     Data.FullBlockList[15][1].idOld="wool[color=orange]";
    Blocks[15][2]=ui->is15StainedGlass;      Data.FullBlockList[15][2].id="orange_stained_glass";
    Blocks[15][3]=ui->is15AcaciaPlanks;      Data.FullBlockList[15][3].id="acacia_planks";      Data.FullBlockList[15][3].idOld="planks[variant=acacia]";
    Blocks[15][4]=ui->is15Pumpkin;      Data.FullBlockList[15][4].id="pumpkin";      Data.FullBlockList[15][4].idOld="pumpkin[variant=north]";
    Blocks[15][5]=ui->is15Terracotta;      Data.FullBlockList[15][5].id="terracotta";        Data.FullBlockList[15][5].idOld="hardened_clay";
    Blocks[15][6]=ui->is15SmoothRedsandstone;      Data.FullBlockList[15][6].id="smooth_red_sandstone";      Data.FullBlockList[15][6].idOld="red_sandstone[type=smooth_red_sandstone]";
    Blocks[15][7]=ui->is15HoneyBlock;      Data.FullBlockList[15][7].id="honey_block";Data.FullBlockList[15][7].version=15;
    Blocks[15][8]=ui->is15Honeycomb;      Data.FullBlockList[15][8].id="honeycomb_block";Data.FullBlockList[15][8].version=15;
    Blocks[15][9]=ui->is15RawCopperBlock;       Data.FullBlockList[15][9].id="raw_copper_block";Data.FullBlockList[15][9].version=17;
    Blocks[15][10]=ui->is15WaxedCopper;         Data.FullBlockList[15][10].id="waxed_copper_block";Data.FullBlockList[15][10].version=17;
    Blocks[15][11]=ui->is15WaxedCutCopper;      Data.FullBlockList[15][11].id="waxed_cut_copper";Data.FullBlockList[15][11].version=17;


    Blocks[16][0]=ui->is16Concrete;      Data.FullBlockList[16][0].id="magenta_concrete";     Data.FullBlockList[16][0].idOld="concrete[color=orange]";
    Blocks[16][1]=ui->is16Wool;      Data.FullBlockList[16][1].id="magenta_wool";     Data.FullBlockList[16][1].idOld="wool[color=magenta]";
    Blocks[16][2]=ui->is16StainedGlass;      Data.FullBlockList[16][2].id="magenta_stained_glass";     Data.FullBlockList[16][2].idOld="stained_glass[color=magenta]";
    Blocks[16][3]=ui->is16PurpurBlock;      Data.FullBlockList[16][3].id="purpur_block";

    Blocks[17][0]=ui->is17Concrete;      Data.FullBlockList[17][0].id="light_blue_concrete";     Data.FullBlockList[17][0].idOld="concrete[color=light_blue]";
    Blocks[17][1]=ui->is17Wool;      Data.FullBlockList[17][1].id="light_blue_wool";     Data.FullBlockList[17][1].idOld="wool[color=light_blue]";
    Blocks[17][2]=ui->is17StainedGlass;      Data.FullBlockList[17][2].id="light_blue_stained_glass";     Data.FullBlockList[17][2].idOld="stained_glass[color=light_blue]";

    Blocks[18][0]=ui->is18Concrete;      Data.FullBlockList[18][0].id="yellow_concrete";     Data.FullBlockList[18][0].idOld="concrete[color=yellow]";
    Blocks[18][1]=ui->is18Wool;      Data.FullBlockList[18][1].id="yellow_wool";     Data.FullBlockList[18][1].idOld="wool[color=yellow]";
    Blocks[18][2]=ui->is18StainedGlass;      Data.FullBlockList[18][2].id="yellow_stained_glass";     Data.FullBlockList[18][2].idOld="stained_glass[color=yellow]";
    Blocks[18][3]=ui->is18HayBlock;      Data.FullBlockList[18][3].id="hay_block[axis=y]";

    Blocks[19][0]=ui->is19Concrete;      Data.FullBlockList[19][0].id="lime_concrete";     Data.FullBlockList[19][0].idOld="concrete[color=lime]";
    Blocks[19][1]=ui->is19Wool;      Data.FullBlockList[19][1].id="lime_wool";     Data.FullBlockList[19][1].idOld="wool[color=lime]";
    Blocks[19][2]=ui->is19StainedGlass;      Data.FullBlockList[19][2].id="lime_stained_glass";     Data.FullBlockList[19][2].idOld="stained_glass[color=lime]";

    Blocks[20][0]=ui->is20Concrete;      Data.FullBlockList[20][0].id="pink_concrete";     Data.FullBlockList[20][0].idOld="concrete[color=pink]";
    Blocks[20][1]=ui->is20Wool;      Data.FullBlockList[20][1].id="pink_wool";     Data.FullBlockList[20][1].idOld="wool[color=pink]";
    Blocks[20][2]=ui->is20StainedGlass;      Data.FullBlockList[20][2].id="pink_stained_glass";     Data.FullBlockList[20][2].idOld="stained_glass[color=pink]";

    Blocks[21][0]=ui->is21Concrete;      Data.FullBlockList[21][0].id="gray_concrete";     Data.FullBlockList[21][0].idOld="concrete[color=gray]";
    Blocks[21][1]=ui->is21Wool;      Data.FullBlockList[21][1].id="gray_wool";     Data.FullBlockList[21][1].idOld="wool[color=gray]";
    Blocks[21][2]=ui->is21StainedGlass;      Data.FullBlockList[21][2].id="gray_stained_glass";     Data.FullBlockList[21][2].idOld="stained_glass[color=gray]";
    Blocks[21][3]=ui->is21TintedGlass;      Data.FullBlockList[21][3].id="tinted_glass";     Data.FullBlockList[21][3].version=17;

    Blocks[22][0]=ui->is22Concrete;      Data.FullBlockList[22][0].id="light_gray_concrete";     Data.FullBlockList[22][0].idOld="concrete[color=silver]";
    Blocks[22][1]=ui->is22Wool;      Data.FullBlockList[22][1].id="light_gray_wool";     Data.FullBlockList[22][1].idOld="wool[color=silver]";
    Blocks[22][2]=ui->is22StainedGlass;      Data.FullBlockList[22][2].id="light_gray_stained_glass";     Data.FullBlockList[22][2].idOld="stained_glass[color=silver]";

    Blocks[23][0]=ui->is23Concrete;      Data.FullBlockList[23][0].id="cyan_concrete";     Data.FullBlockList[23][0].idOld="concrete[color=cyan]";
    Blocks[23][1]=ui->is23Wool;      Data.FullBlockList[23][1].id="cyan_wool";     Data.FullBlockList[23][1].idOld="wool[color=cyan]";
    Blocks[23][2]=ui->is23StainedGlass;      Data.FullBlockList[23][2].id="cyan_stained_glass";     Data.FullBlockList[23][2].idOld="stained_glass[color=cyan]";
    Blocks[23][3]=ui->is23Prismarine;      Data.FullBlockList[23][3].id="prismarine";    Data.FullBlockList[23][3].idOld="prismarine[variant=prismarine]";

    Blocks[24][0]=ui->is24Concrete;      Data.FullBlockList[24][0].id="purple_concrete";     Data.FullBlockList[24][0].idOld="concrete[color=purple]";
    Blocks[24][1]=ui->is24Wool;      Data.FullBlockList[24][1].id="purple_wool";     Data.FullBlockList[24][1].idOld="wool[color=purple]";
    Blocks[24][2]=ui->is24StainedGlass;      Data.FullBlockList[24][2].id="purple_stained_glass";     Data.FullBlockList[24][2].idOld="stained_glass[color=purple]";
    Blocks[24][3]=ui->is24AmethystBlock;        Data.FullBlockList[24][3].id="amethyst_block";       Data.FullBlockList[24][3].version=17;

    Blocks[25][0]=ui->is25Concrete;      Data.FullBlockList[25][0].id="blue_concrete";     Data.FullBlockList[25][0].idOld="concrete[color=blue]";
    Blocks[25][1]=ui->is25Wool;      Data.FullBlockList[25][1].id="blue_wool";     Data.FullBlockList[25][1].idOld="wool[color=blue]";
    Blocks[25][2]=ui->is25StainedGlass;      Data.FullBlockList[25][2].id="blue_stained_glass";     Data.FullBlockList[25][2].idOld="stained_glass[color=blue]";

    Blocks[26][0]=ui->is26Concrete;      Data.FullBlockList[26][0].id="brown_concrete";     Data.FullBlockList[26][0].idOld="concrete[color=brown]";
    Blocks[26][1]=ui->is26Wool;      Data.FullBlockList[26][1].id="brown_wool";     Data.FullBlockList[26][1].idOld="wool[color=brown]";
    Blocks[26][2]=ui->is26StainedGlass;      Data.FullBlockList[26][2].id="brown_stained_glass";     Data.FullBlockList[26][2].idOld="stained_glass[color=brown]";
    Blocks[26][3]=ui->is26DarkOakPlanks;      Data.FullBlockList[26][3].id="dark_oak_planks";      Data.FullBlockList[26][3].idOld="planks[variant=dark_oak]";
    Blocks[26][4]=ui->is26SoulSand;      Data.FullBlockList[26][4].id="soul_sand";
    Blocks[26][5]=ui->is26SoulSoil;      Data.FullBlockList[26][5].id="soul_soil";Data.FullBlockList[26][5].version=16;

    Blocks[27][0]=ui->is27Concrete;      Data.FullBlockList[27][0].id="green_concrete";     Data.FullBlockList[27][0].idOld="concrete[color=green]";
    Blocks[27][1]=ui->is27Wool;      Data.FullBlockList[27][1].id="green_wool";     Data.FullBlockList[27][1].idOld="wool[color=green]";
    Blocks[27][2]=ui->is27StainedGlass;      Data.FullBlockList[27][2].id="green_stained_glass";     Data.FullBlockList[27][2].idOld="stained_glass[color=green]";
    Blocks[27][3]=ui->is27KelpBlock;      Data.FullBlockList[27][3].id="dried_kelp_block";Data.FullBlockList[27][3].version=13;

    Blocks[28][0]=ui->is28Concrete;      Data.FullBlockList[28][0].id="red_concrete";     Data.FullBlockList[28][0].idOld="concrete[color=red]";
    Blocks[28][1]=ui->is28Wool;      Data.FullBlockList[28][1].id="red_wool";     Data.FullBlockList[28][1].idOld="wool[color=red]";
    Blocks[28][2]=ui->is28StainedGlass;      Data.FullBlockList[28][2].id="red_stained_glass";     Data.FullBlockList[28][2].idOld="stained_glass[color=red]";
    Blocks[28][3]=ui->is28Bricks;      Data.FullBlockList[28][3].id="bricks";        Data.FullBlockList[28][3].idOld="brick_block";
    Blocks[28][4]=ui->is28RedMushroomBlock;      Data.FullBlockList[28][4].id="red_mushroom_block[east=true,west=true,north=true,south=true,up=true,down=true]";     Data.FullBlockList[28][4].idOld="red_mushroom_block[variant=all_outside]";
    Blocks[28][5]=ui->is28WartBlock;      Data.FullBlockList[28][5].id="nether_wart_block";
    Blocks[28][6]=ui->is28Shroomlight;      Data.FullBlockList[28][6].id="shroomlight";Data.FullBlockList[28][6].version=16;

    Blocks[29][0]=ui->is29Concrete;      Data.FullBlockList[29][0].id="black_concrete";      Data.FullBlockList[29][0].idOld="concrete[color=black]";
    Blocks[29][1]=ui->is29Wool;      Data.FullBlockList[29][1].id="black_wool";     Data.FullBlockList[29][1].idOld="wool[color=black]";
    Blocks[29][2]=ui->is29StainedGlass;      Data.FullBlockList[29][2].id="black_stained_glass";     Data.FullBlockList[29][2].idOld="stained_glass[color=black]";
    Blocks[29][3]=ui->is29Obsidian;      Data.FullBlockList[29][3].id="obsidian";
    Blocks[29][4]=ui->is29CoalBlock;      Data.FullBlockList[29][4].id="coal_block";
    Blocks[29][5]=ui->is29Basalt;      Data.FullBlockList[29][5].id="polished_basalt[axis=y]";Data.FullBlockList[29][5].version=16;
    Blocks[29][6]=ui->is29BlackStone;      Data.FullBlockList[29][6].id="polished_blackstone";Data.FullBlockList[29][6].version=16;

    Blocks[30][0]=ui->is30GoldBlock;      Data.FullBlockList[30][0].id="gold_block";
    Blocks[30][1]=ui->is30GoldPlate;      Data.FullBlockList[30][1].id="light_weighted_pressure_plate[power=0]";
    Blocks[30][2]=ui->is30RawGoldBlock;     Data.FullBlockList[30][2].id="raw_gold_block";       Data.FullBlockList[30][2].version=17;

    Blocks[31][0]=ui->is31DiamondBlock;      Data.FullBlockList[31][0].id="diamond_block";
    Blocks[31][1]=ui->is31PrismarineBricks;      Data.FullBlockList[31][1].id="prismarine_bricks";       Data.FullBlockList[31][1].idOld="prismarine[variant=prismarine_bricks]";
    Blocks[31][2]=ui->is31DarkPrismarine;      Data.FullBlockList[31][2].id="dark_prismarine";Data.FullBlockList[31][2].idOld="prismarine[variant=dark_prismarine]";

    Blocks[32][0]=ui->is32LapisBlock;      Data.FullBlockList[32][0].id="lapis_block";

    Blocks[33][0]=ui->is33EmeraldBlock;         Data.FullBlockList[33][0].id="emerald_block";

    Blocks[34][0]=ui->is34Podzol;         Data.FullBlockList[34][0].id="podzol[snowy=false]";        Data.FullBlockList[34][0].idOld="dirt[variant=podzol,snowy=false]";
    Blocks[34][1]=ui->is34SprucePlanks;         Data.FullBlockList[34][1].id="spruce_planks";      Data.FullBlockList[34][1].idOld="planks[variant=spruce]";
    Blocks[34][2]=ui->is34Campfire;         Data.FullBlockList[34][2].id="campfire[facing=north,lit=true,signal_fire=false,waterlogged=false]";Data.FullBlockList[34][2].version=14;

    Blocks[35][0]=ui->is35Netherrack;         Data.FullBlockList[35][0].id="netherrack";
    Blocks[35][1]=ui->is35NetherBricks;         Data.FullBlockList[35][1].id="nether_bricks";        Data.FullBlockList[35][1].idOld="nether_brick";
    Blocks[35][2]=ui->is35Magma;         Data.FullBlockList[35][2].id="magma_block";     Data.FullBlockList[35][2].idOld="magma";

    Blocks[36][0]=ui->is36Terracotta;           Data.FullBlockList[36][0].id="white_terracotta";     Data.FullBlockList[36][0].idOld="stained_hardened_clay[color=white]";
    Blocks[36][1]=ui->is36Calcite;          Data.FullBlockList[36][1].id="calcite";      Data.FullBlockList[36][1].version=17;

    Blocks[37][0]=ui->is37Terracotta;           Data.FullBlockList[37][0].id="orange_terracotta";     Data.FullBlockList[37][0].idOld="stained_hardened_clay[color=orange]";
    Blocks[38][0]=ui->is38Terracotta;           Data.FullBlockList[38][0].id="magenta_terracotta";     Data.FullBlockList[38][0].idOld="stained_hardened_clay[color=magenta]";
    Blocks[39][0]=ui->is39Terracotta;           Data.FullBlockList[39][0].id="light_blue_terracotta";     Data.FullBlockList[39][0].idOld="stained_hardened_clay[color=light_blue]";
    Blocks[40][0]=ui->is40Terracotta;           Data.FullBlockList[40][0].id="yellow_terracotta";     Data.FullBlockList[40][0].idOld="stained_hardened_clay[color=yellow]";
    Blocks[41][0]=ui->is41Terracotta;           Data.FullBlockList[41][0].id="lime_terracotta";     Data.FullBlockList[41][0].idOld="stained_hardened_clay[color=lime]";
    Blocks[42][0]=ui->is42Terracotta;           Data.FullBlockList[42][0].id="pink_terracotta";     Data.FullBlockList[42][0].idOld="stained_hardened_clay[color=pink]";
    Blocks[43][0]=ui->is43Terracotta;           Data.FullBlockList[43][0].id="gray_terracotta";     Data.FullBlockList[43][0].idOld="stained_hardened_clay[color=gray]";
    Blocks[44][0]=ui->is44Terracotta;           Data.FullBlockList[44][0].id="light_gray_terracotta";     Data.FullBlockList[44][0].idOld="stained_hardened_clay[color=silver]";
    Blocks[44][1]=ui->is44WaxExposedCopper;         Data.FullBlockList[44][1].id="waxed_exposed_copper";     Data.FullBlockList[44][1].version=17;
    Blocks[44][2]=ui->is44WaxCutExposedCopper;          Data.FullBlockList[44][2].id="waxed_exposed_cut_copper";     Data.FullBlockList[44][2].version=17;

    Blocks[45][0]=ui->is45Terracotta;           Data.FullBlockList[45][0].id="cyan_terracotta";     Data.FullBlockList[45][0].idOld="stained_hardened_clay[color=cyan]";
    Blocks[46][0]=ui->is46Terracotta;           Data.FullBlockList[46][0].id="purple_terracotta";     Data.FullBlockList[46][0].idOld="stained_hardened_clay[color=purple]";
    Blocks[47][0]=ui->is47Terracotta;           Data.FullBlockList[47][0].id="blue_terracotta";     Data.FullBlockList[47][0].idOld="stained_hardened_clay[color=blue]";
    Blocks[48][0]=ui->is48Terracotta;           Data.FullBlockList[48][0].id="brown_terracotta";     Data.FullBlockList[48][0].idOld="stained_hardened_clay[color=brown]";
    Blocks[48][1]=ui->is48DripstoneBlock;           Data.FullBlockList[48][1].id="dripstone_block";      Data.FullBlockList[48][1].version=17;

    Blocks[49][0]=ui->is49Terracotta;           Data.FullBlockList[49][0].id="green_terracotta";     Data.FullBlockList[49][0].idOld="stained_hardened_clay[color=green]";
    Blocks[50][0]=ui->is50Terracotta;           Data.FullBlockList[50][0].id="red_terracotta";     Data.FullBlockList[50][0].idOld="stained_hardened_clay[color=red]";
    Blocks[51][0]=ui->is51Terracotta;           Data.FullBlockList[51][0].id="black_terracotta";     Data.FullBlockList[51][0].idOld="stained_hardened_clay[color=black]";

    Blocks[52][0]=ui->is52CrimsonNylium;           Data.FullBlockList[52][0].id="crimson_nylium";Data.FullBlockList[52][0].version=16;

    Blocks[53][0]=ui->is53CrimsonPlanks;           Data.FullBlockList[53][0].id="crimson_planks";Data.FullBlockList[53][0].version=16;
    Blocks[53][1]=ui->is53StrippedCrimsonLog;           Data.FullBlockList[53][1].id="stripped_crimson_stem[axis=y]";Data.FullBlockList[53][1].version=16;

    Blocks[54][0]=ui->is54CrimsonStem;           Data.FullBlockList[54][0].id="crimson_hyphae[axis=y]";Data.FullBlockList[54][0].version=16;
    Blocks[54][1]=ui->is54StrippedCrimsonStem;           Data.FullBlockList[54][1].id="stripped_crimson_hyphae[axis=y]";Data.FullBlockList[54][1].version=16;

    Blocks[55][0]=ui->is55WarpedNylium;           Data.FullBlockList[55][0].id="warped_nylium";Data.FullBlockList[55][0].version=16;
    Blocks[55][1]=ui->is55WaxedOxidedCopper;            Data.FullBlockList[55][1].id="waxed_oxidized_copper";    Data.FullBlockList[55][1].version=17;
    Blocks[55][2]=ui->is55WaxedCutOxidedCopper;            Data.FullBlockList[55][2].id="waxed_oxidized_cut_copper";    Data.FullBlockList[55][2].version=17;


    Blocks[56][0]=ui->is56WarpedPlanks;           Data.FullBlockList[56][0].id="warped_planks";Data.FullBlockList[56][0].version=16;
    Blocks[56][1]=ui->is56StrippedWarpedLog;           Data.FullBlockList[56][1].id="stripped_warped_stem[axis=y]";Data.FullBlockList[56][1].version=16;
    Blocks[56][2]=ui->is56WaxedWeatheredCopper;         Data.FullBlockList[56][2].id="waxed_weathered_copper";Data.FullBlockList[56][2].version=17;
    Blocks[56][3]=ui->is56WaxedCutWeatheredCopper;          Data.FullBlockList[56][3].id="waxed_oxidized_cut_copper";Data.FullBlockList[56][3].version=17;


    Blocks[57][0]=ui->is57WarpedStem;           Data.FullBlockList[57][0].id="warped_hyphae[axis=y]";Data.FullBlockList[57][0].version=16;
    Blocks[57][1]=ui->is57StrippedWarpedStem;           Data.FullBlockList[57][1].id="stripped_warped_hyphae[axis=y]";Data.FullBlockList[57][1].version=16;

    Blocks[58][0]=ui->is58WarpedWartBlock;           Data.FullBlockList[58][0].id="warped_wart_block";Data.FullBlockList[58][0].version=16;

    Blocks[59][0]=ui->is59Deepslate;        Data.FullBlockList[59][0].id="deepslate";    Data.FullBlockList[59][0].version=17;
    Blocks[59][1]=ui->is59ChiseledDeepslate;    Data.FullBlockList[59][1].id="chiseled_deepslate";   Data.FullBlockList[59][1].version=17;
    Blocks[59][2]=ui->is59PolishedDeepslate;    Data.FullBlockList[59][2].id="polished_deepslate";   Data.FullBlockList[59][2].version=17;
    Blocks[59][3]=ui->is59DeepslateBricks;      Data.FullBlockList[59][3].id="deepslate_bricks";     Data.FullBlockList[59][3].version=17;
    Blocks[59][4]=ui->is59DeepslateTiles;       Data.FullBlockList[59][4].id="deepslate_tiles";      Data.FullBlockList[59][4].version=17;
    Blocks[59][5]=ui->is59CobbledDeepslate;     Data.FullBlockList[59][5].id="cobbled_deepslate";        Data.FullBlockList[59][5].version=17;

    Blocks[60][0]=ui->is60RawIronBlock;     Data.FullBlockList[60][0].id="raw_iron_block";       Data.FullBlockList[60][0].version=17;
    for(short r=0;r<64;r++)
        for(short c=0;c<9;c++)
            if(Blocks[r][c]==NULL)Data.FullBlockList[r][c].version=255;

}


#endif



