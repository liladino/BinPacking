TIMEFORMAT="%R"

for j in {0..5}
do
    echo ""
    echo "algo $j"
    for i in {1..10}
    do
        time ./Packing_cpp/packer.exe -i data/many_items.txt --shipEverything --algorithm "$j" --firstFit > /dev/null
    done 
done