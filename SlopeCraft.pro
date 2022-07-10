TEMPLATE = subdirs

SUBDIRS += \
    GAConverter \
    SlopeCraftL \
    BlockListManager \
    SlopeCraftMain \
    imageCutter

BlockListManager.depends = SlopeCraftL
SlopeCraftMain.depends = BlockListManager SlopeCraftL
