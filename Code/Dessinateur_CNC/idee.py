

X = 0
Y = 0
Z = 0

nX = 1
nY = 1
nZ = 1

def stepped(n, A, B):
    return round(n * (A / (B + 1)))

plusGrand = X
if (plusGrand < Y) plusGrand = Y
if (plusGrand < Z) plusGrand = Z

# Malloc the 2 lists of uint32_t masks (AND and OR)
for (stepIndex of range(plusGrand)):
    isStep = floor(stepped(nX, plusGrand, X) / stepIndex)
    nX += isStep
    maskStepX = getNextMaskStepX() * isStep
    **listMaskAND &= maskStepX ^ 0xFF
    **listMaskOR |= maskStepX

    isStep = floor(stepped(nY, plusGrand, Y) / stepIndex)
    nY += isStep
    maskStepY = getNextMaskStepY() * isStep
    **listMaskAND &= (maskStepY ^ 0xFF) << 4
    **listMaskOR |= (maskStepY) << 4

    isStep = floor(stepped(nZ, plusGrand, Z) / stepIndex)
    nZ += isStep
    maskStepZ = getNextMaskStepZ() * isStep
    **listMaskAND &= (maskStepZ ^ 0xFF) << 8
    **listMaskOR |= (maskStepZ) << 8
    
        
