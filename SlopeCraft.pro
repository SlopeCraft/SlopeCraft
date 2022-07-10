TEMPLATE = subdirs

SUBDIRS += \
    GAConverter \
    SlopeCraftL \
    BlockListManager \
    SlopeCraftMain \
    imageCutter

BlockListManager.depends = SlopeCraftL
SlopeL.depends = GAConverter
SlopeCraftMain.depends = BlockListManager SlopeCraftL
