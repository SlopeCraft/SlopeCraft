TEMPLATE = subdirs

SUBDIRS += \
    SlopeCraftL \
    BlockListManager \
    SlopeCraftMain \
    imageCutter

BlockListManager.depends = SlopeCraftL
SlopeCraftMain.depends = BlockListManager SlopeCraftL
