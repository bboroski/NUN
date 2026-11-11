#!/usr/bin/env bash
set -Eeuo pipefail

SCRIPT_PATH="$(realpath -- "$0")"

usage() {
    echo "Usage: sudo $0 <NUN.iso> <whole-disk-device>" >&2
    echo "Example: sudo $0 build/NUN.iso /dev/sdX" >&2
}

if [[ $# -ne 2 ]]; then
    usage
    exit 2
fi

IMAGE="$(realpath -- "$1")"
DEVICE="$(readlink -f -- "$2")"

[[ -f "$IMAGE" && -r "$IMAGE" ]] || {
    echo "Image not found or unreadable: $IMAGE" >&2
    exit 2
}
[[ -b "$DEVICE" ]] || {
    echo "Target is not a block device: $DEVICE" >&2
    exit 2
}
if [[ $EUID -ne 0 ]]; then
    command -v sudo >/dev/null || {
        echo "Run as root or install sudo." >&2
        exit 2
    }
    exec sudo -- "$SCRIPT_PATH" "$IMAGE" "$DEVICE"
fi

command -v lsblk >/dev/null
command -v findmnt >/dev/null
command -v blockdev >/dev/null
command -v dd >/dev/null

[[ "$(lsblk -dn -o TYPE "$DEVICE" | xargs)" == "disk" ]] || {
    echo "Select a whole disk (for example /dev/sdX), not a partition." >&2
    exit 2
}
[[ "$(blockdev --getro "$DEVICE")" == "0" ]] || {
    echo "Target device is read-only: $DEVICE" >&2
    exit 2
}

IMAGE_SIZE="$(stat -c '%s' "$IMAGE")"
DEVICE_SIZE="$(blockdev --getsize64 "$DEVICE")"
(( DEVICE_SIZE >= IMAGE_SIZE )) || {
    echo "Target is too small: need $IMAGE_SIZE bytes, have $DEVICE_SIZE." >&2
    exit 2
}

ROOT_SOURCE="$(findmnt -n -o SOURCE / || true)"
if [[ -n "$ROOT_SOURCE" ]]; then
    while IFS= read -r ancestor; do
        [[ -n "$ancestor" ]] || continue
        if [[ "$(readlink -f -- "$ancestor" 2>/dev/null || true)" == "$DEVICE" ]]; then
            echo "Refusing to overwrite the disk that contains the running system: $DEVICE" >&2
            exit 3
        fi
    done < <(lsblk -spn -o PATH "$ROOT_SOURCE" 2>/dev/null || true)
fi

while IFS= read -r mountpoint; do
    if [[ -n "$mountpoint" ]]; then
        echo "Target has mounted filesystem(s). Unmount them first: $mountpoint" >&2
        exit 3
    fi
done < <(lsblk -nr -o MOUNTPOINT "$DEVICE")

echo
echo "NUN USB installer"
echo "Image:  $IMAGE ($IMAGE_SIZE bytes)"
echo "Target:"
lsblk -o NAME,PATH,SIZE,MODEL,TRAN,TYPE,MOUNTPOINT "$DEVICE"
echo
echo "WARNING: every existing partition and file on $DEVICE will be overwritten."
read -r -p "Type exactly: ERASE $DEVICE : " confirmation
[[ "$confirmation" == "ERASE $DEVICE" ]] || {
    echo "Cancelled. No data was written."
    exit 1
}

echo "[NUN] Writing image to $DEVICE..."
dd if="$IMAGE" of="$DEVICE" bs=4M iflag=fullblock conv=fsync status=progress
sync

echo "[NUN] Image written. Safely eject the device before rebooting."
echo "[NUN] Select the USB device from your firmware boot menu."
