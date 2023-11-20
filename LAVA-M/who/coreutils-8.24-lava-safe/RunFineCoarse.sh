rm -rf out-AFL-Both out-AFL-Fine out-AFL-Coarse

gnome-terminal -e "../../../afl-fuzz-org -i ../fuzzer_input -o out-AFL-Both ./installed-Org/bin/who -a @@"

gnome-terminal -e "../../../afl-Fine -i ../fuzzer_input -o out-AFL-Fine ./installed-Org/bin/who -a @@"

gnome-terminal -e "../../../afl-Coarse -i ../fuzzer_input -o out-AFL-Coarse ./installed-Org/bin/who -a @@"


