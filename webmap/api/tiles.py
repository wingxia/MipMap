from fastapi import APIRouter, HTTPException, Response
from PIL import Image

from core.config import TILE_CACHE_MAX_AGE, WORLDS_DIR


router = APIRouter(prefix="/api", tags=["tiles"])
TILE_SIZE = 256
BASE_ZOOM = 4


@router.get("/tiles/{dimension}/{z}/{x}/{y}")
async def getTile(dimension: str, z: int, x: int, y: int):
    try:
        tilePath = WORLDS_DIR / dimension / "tiles" / f"zoom-{z}" / f"({x})-({y}).png"
        if not tilePath.exists():
            _ensureTile(dimension, z, x, y)

        with open(tilePath, "rb") as f:
            tileData = f.read()

    except FileNotFoundError:
        raise HTTPException(status_code=404, detail="Tile not found")
    
    return Response(
        content=tileData,
        media_type="image/png",
        headers={"Cache-Control": f"public, max-age={TILE_CACHE_MAX_AGE}"}
    )


def _ensureTile(dimension: str, zoom: int, x: int, y: int) -> None:
    if zoom >= BASE_ZOOM:
        return

    sourceZoom = zoom + 1
    sourcePath = WORLDS_DIR / dimension / "tiles" / f"zoom-{sourceZoom}"
    targetPath = WORLDS_DIR / dimension / "tiles" / f"zoom-{zoom}"
    targetPath.mkdir(parents=True, exist_ok=True)

    if sourceZoom < BASE_ZOOM:
        for dx in range(2):
            for dy in range(2):
                _ensureTile(dimension, sourceZoom, x * 2 + dx, y * 2 + dy)

    tilePath = targetPath / f"({x})-({y}).png"
    if tilePath.exists():
        return

    targetImage = Image.new("RGBA", (TILE_SIZE, TILE_SIZE), (0, 0, 0, 0))
    for dx in range(2):
        for dy in range(2):
            sourceX = x * 2 + dx
            sourceY = y * 2 + dy
            sourceTilePath = sourcePath / f"({sourceX})-({sourceY}).png"

            if sourceTilePath.exists():
                sourceImage = Image.open(sourceTilePath)
                resized = sourceImage.resize((TILE_SIZE // 2, TILE_SIZE // 2), Image.Resampling.LANCZOS)
                targetImage.paste(resized, (dx * TILE_SIZE // 2, dy * TILE_SIZE // 2))

    if targetImage.getbbox():
        targetImage.save(tilePath, optimize=True)
