import csv

in_file = 'escape_table.csv'
out_file = 'escape_table.txt'

start_declaration = 'const ASCII_USB_RELATION_t escape_string_hid_map[ESC_MAP_SIZE] = {\n'

pattern_str_mod = """\t\t{{ .ascii_rep = "{0}",
\t\t  .usage_code_rep = {{.modifiers = {1},\n"""
pattern_key_start = "\t\t\t\t\t\t\t "
pattern_keys = ".key{0} = {1}"
pattern_keys_join = ", "
pattern_keys_end = " }}, \n"

end_declaration = '};\n'

length_declaration = "#define ESC_MAP_SIZE {}"

header_ln = True
line_count = 0
with open(in_file, 'r') as src, open(out_file, 'w') as dst:
    reader = csv.reader(src, delimiter=',')
    dst.write(start_declaration)

    for row in reader:
        if not header_ln:
            dst.write(pattern_str_mod.format(row[0], row[1]))
            dst.write(pattern_key_start + pattern_keys.format(1, row[2]))

            key_count = 2
            assert len(row) <= 8  # six keys max
            for key in row[3:]:
                dst.write(pattern_keys_join)
                dst.write(pattern_keys.format(key_count, key))
                key_count += 1

            dst.write(pattern_keys_end)
            line_count += 1
        else:
            header_ln = False

    dst.write(end_declaration)
    dst.write("\n\n\n"+length_declaration.format(line_count))
