TEMPLATE = subdirs

SUBDIRS += \
    GAConverter \
    MapViewer \
    SlopeCraftL \
    BlockListManager \
    SlopeCraftMain \
    imageCutter

BlockListManager.depends = SlopeCraftL
SlopeL.depends = GAConverter
SlopeCraftMain.depends = BlockListManager SlopeCraftL
