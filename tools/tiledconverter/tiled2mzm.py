import json
import os

def to_byte(n: int) -> int:
    return n % 0x100

def u16_list_to_le_bytes(l: List[int]) -> bytes:
    return bytes().join([i.to_bytes(2, 'little') for i in l])

def flush_lonely_bytes(lonely_bytes, output):
    if lonely_bytes:
        # FIXME Need to split if it's larger than 0x80
        assert len(lonely_bytes) < 0x80
        output.append(len(lonely_bytes))
        for byte in lonely_bytes:
            output.append(byte)
        lonely_bytes.clear()

# Input and output are uint16_t streams, the input should be turned into a matching byte stream.
def compress_rle_pass(values: List[int], counts: List[int], use_one_byte: bool) -> bytes:
    output: bytearray = bytearray()

    lonely_bytes: List[int] = list()
    output.append(1 if use_one_byte else 2)
    for value, count in zip(values, counts):
        while count > 0:
            if use_one_byte:
                # FIXME Is that right for 2 bytes too?
                if count <= 2:
                    while count > 0:
                        lonely_bytes.append(value)
                        count -= 1
                else:
                    flush_lonely_bytes(lonely_bytes, output)
                    use = min(count, 0x7f)
                    output.append(0x80 + use)
                    output.append(value)
                    count -= use
            else:
                use = min(count, 0x7fff)
                output.append(0x80 + (use >> 8))
                output.append(use % 0x100)
                output.append(value)
                count -= use
    if use_one_byte:
        flush_lonely_bytes(lonely_bytes, output)

    # Add the pass terminator
    if use_one_byte:
        output.append(0)
    else:
        output.append(0)
        output.append(0)

    return bytes(output)

# Input and output are uint16_t streams, the input should be turned into a matching byte stream.
def compress_rle(input: bytes) -> bytes:
    output: bytearray = bytearray()

    # do two passes for even and odd bytes
    for rle_pass in range(0, 2):
        # get counts of consecutive values
        values: List[int] = list()
        counts: List[int] = list()

        prev: int = input[rle_pass]
        values.append(prev)
        count: int = 1

        for i in range(rle_pass + 2, len(input), 2):
            val: int = input[i]
            if val == prev: # if consecutive values are equal
                count += 1
            else: # if consecutive values are different
                values.append(val)
                counts.append(count)
                prev = val
                count = 1
        counts.append(count)

        with_one_byte = compress_rle_pass(values, counts, True)
        with_two_bytes = compress_rle_pass(values, counts, False)

        output.extend(with_one_byte if len(with_one_byte) <= len(with_two_bytes) else with_two_bytes)

    return bytes(output)

def compress_clipdata(width: int, height: int, clipdata: List[int]) -> bytes:
    return bytes([width, height]) + compress_rle(u16_list_to_le_bytes(clipdata))

def extract_data(room_tmj_file, area, room) -> None:
    with open(room_tmj_file, "r") as input_f:
        room_tmj = json.load(input_f)

        clipdata_layer = [l for l in room_tmj["layers"] if l["name"] == "Clipdata"]
        assert len(clipdata_layer) == 1

        width = clipdata_layer[0]["width"]
        height = clipdata_layer[0]["height"]
        clipdata = clipdata_layer[0]["data"]

        clipdata_rle = compress_clipdata(width, height, clipdata)
        clipdata_rle_str = ",".join([str(i) + "u" for i in clipdata_rle])

        room_inc_file = "include/extracted/data/rooms/{}/{}_clipdata.gfx.inc".format(area, room)
        with open(room_inc_file, "w") as output_f:
            output_f.write(clipdata_rle_str)

if __name__ == "__main__":
    tiled_dir = "tiled"
    for area in os.listdir(tiled_dir):
        area_dir = os.path.join(tiled_dir, area)
        if os.path.isdir(area_dir):
            for room in os.listdir(area_dir):
                room_tmj_file = os.path.join(area_dir, room)
                if room_tmj_file.endswith("tmj"):
                    room = os.path.splitext(room)[0]
                    extract_data(room_tmj_file, area, room)
