import re

clipdata_tsj_template = '''{{ "columns":1,
 "image":"clipdata.png",
 "imageheight":2800,
 "imagewidth":16,
 "margin":0,
 "name":"clipdata",
 "spacing":0,
 "tilecount":175,
 "tiledversion":"1.12.2",
 "tileheight":16,
 "tiles":[{}],
 "tilewidth":16,
 "type":"tileset",
 "version":"1.10"
}}'''

clipdata_tile_properties_template = '''
        {{
         "id":{},
         "type":"{}"
        }}'''

def extract_clipdata_enum(content: str) -> List[str]:
    enum_names_list: List[str] = list()

    enum_str: str = re.search(r"MAKE_ENUM\(u16, Clipdata\) \{(.*?)\}", content, re.MULTILINE | re.DOTALL).group(0)
    enum_entry_value: int = 0
    for entry in re.findall(r"CLIPDATA_(.*),", enum_str):
        # If the entry has a set value, fill in the blanks
        # FIXME Only supports base 10 literals
        entry_with_skip = re.match(r"(.*?)\s*=\s*(\d+)", entry)
        if entry_with_skip:
            entry = entry_with_skip.group(1)
            skip_to_value: int = int(entry_with_skip.group(2))
            while enum_entry_value < skip_to_value:
                enum_names_list.append(None)
                enum_entry_value += 1
        enum_names_list.append(entry.lower())
        enum_entry_value += 1

    return enum_names_list

if __name__ == "__main__":
    enum_names_list: List[str] = list()

    with open("include/constants/clipdata.h", "r") as intput_f:
        content: str = intput_f.read()
        enum_names_list = extract_clipdata_enum(content)

    tile_properties = ",".join([clipdata_tile_properties_template.format(i, name) for i, name in enumerate(enum_names_list[1:]) if name is not None])
    tsj = clipdata_tsj_template.format(tile_properties)
    print(tsj)
