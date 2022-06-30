#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <bitset>
#include <map>
#include <thread>
#include <mutex>

template<typename T>
void printVector(std::vector<T> vec){
    for (size_t i = 0; i < vec.size(); i++)
    {
        std::cout<<vec[i];
        if (i < vec.size() - 1) {
            std::cout<<", ";
        }
    }
    std::cout<<std::endl;
}

template<typename T>
void printMatrix(std::vector<std::vector<T>> mat){
    for (size_t i = 0; i < mat.size(); i++)
    {
        printVector(mat[i]);
    }
}

class Color
{
    public:
    Color():r(0), g(0), b(0){}
    Color(const Color& color) {
        r = color.r;
        g = color.g;
        b = color.b;
    }
    Color(uint32_t color){ 
        Color c = Color::decode(color);
        r = c.r;
        g = c.g;
        b = c.b;
    }
    Color(int _r, int _g, int _b){
        r = uint8_t(_r);
        g = uint8_t(_g);
        b = uint8_t(_b);
    }
    
    uint32_t encode(void) const{
        return (uint32_t(r)<<16) + (uint32_t(g)<<8) + uint32_t(b);
    }
    static Color decode(uint32_t val) {
        uint8_t r = (val>>16) & 0xff;
        uint8_t g = (val>>8) & 0xff;
        uint8_t b = val & 0xff;
        return Color(r, g, b);
    }
    void show(void) {
        std::cout<<"(r, g, b) = ("<<uint32_t(r)<<", "<<uint32_t(g)<<", "<<uint32_t(b)<<")"<<std::endl;
    }

    bool operator==(const Color& color) const {
        return (r == color.r) && (g == color.g) && (b ==color.b);
    }

    public:
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

class ColorCraftingTable {
    public:
    ColorCraftingTable(void){
        totalRed = 0;
        totalGreen = 0;
        totalBlue = 0;
        maxSum = 0;
        numColors = 0;
    }

    public:
    void addColor(const Color& color) {
        totalRed += color.r;
        totalGreen += color.g;
        totalBlue += color.b;
        maxSum += std::max(color.r, std::max(color.g, color.b));
        numColors += 1;
    }

    Color craft() {
        uint32_t avgRed = totalRed / numColors;
        uint32_t avgGreen = totalGreen / numColors;
        uint32_t avgBlue = totalBlue / numColors;
        float avgOfMax = maxSum*1.0 / numColors;
        uint32_t maxAvg = std::max(avgRed, std::max(avgGreen, avgBlue));
        // std::cout<<numColors<<","<<avgRed<<","<<avgGreen<<","<<avgBlue<<","<<avgOfMax<<","<<maxAvg<<","<<std::endl;
        return Color(
            uint8_t(float(avgRed)*avgOfMax/maxAvg),
            uint8_t(float(avgGreen)*avgOfMax/maxAvg), 
            uint8_t(float(avgBlue)*avgOfMax/maxAvg)
        );
    }

    private:
    uint32_t totalRed;
    uint32_t totalGreen;
    uint32_t totalBlue;
    uint32_t maxSum;
    uint32_t numColors;
};

void generateCompose(int total, int choice, std::vector<int>& current, std::vector<std::vector<int>>& ans){
    if (choice <= 0) {
        std::vector<int> compose;
        for (size_t i = 0; i < current.size(); i++)
        {
            compose.push_back(current[current.size() - 1 - i]);
        }
        ans.push_back(compose);
        return;
    }
    if (total<=0 || choice <=0 || choice > total) return;
    // do not choose current number
    generateCompose(total-1, choice, current, ans);
    // choose current number.
    current.push_back(total);
    generateCompose(total-1, choice-1, current, ans);
    current.pop_back();
}

std::vector<int> composeToRecipe(const std::vector<int>& compose, int num) {
    std::vector<int> pre = compose;
    std::vector<int> post = compose;
    pre.insert(pre.begin(), 0);
    post.push_back(num);
    std::vector<int> recipe;
    for (size_t i = 1; i < pre.size(); i++)
    {
        recipe.push_back(post[i] - pre[i] - 1);
    }
    return recipe;
}

class Recipe {
    public:
    Recipe(const std::vector<int>& _counts){
        counts = _counts;
    }

    public:
    uint64_t encode(void) {
        uint64_t val = 0;
        uint64_t bits = 1;
        for (size_t i = 0; i < counts.size(); i++)
        {
            val += bits * counts[i];
            bits *= 9;
        }
        return val;
    }
    static Recipe decode(uint64_t encoded) {
        std::vector<int> counts;
        for (size_t i = 0; i < 16; i++)
        {
            counts.push_back(int(encoded%9));
            encoded /= 9;
        }
        return Recipe(counts);
    }
    void preCraft(const std::vector<Color>& colorlist) {
        for (size_t i = 0; i < counts.size(); i++)
        {
            for (int j = 0; j < counts[i]; j++)
            {
                table.addColor(colorlist[i]);
            }
        }
    }

    Color getTableColor(void) {
        return table.craft();
    }

    Color getCraftedArmorColor(const Color armorColor) {
        ColorCraftingTable temp = table;
        if (!(armorColor == Color(0, 0, 0))) {
            temp.addColor(armorColor);
        }
        return temp.craft();
    }

    std::vector<int> getCounts(void){
        return counts;
    }

    private:
    std::vector<int> counts;
    ColorCraftingTable table;

};

void dumpPossibleRecipe(const std::string& path) {
    std::vector<std::vector<int>> ans;
    std::vector<int> current;
    std::fstream recipes;
    recipes.open(path, std::fstream::out|std::fstream::binary);
    generateCompose(24, 16, current, ans);
    size_t counts = 0;
    for (size_t i = 0; i < ans.size(); i++)
    {
        Recipe recipe(composeToRecipe(ans[i], 25));
        uint64_t encoded = recipe.encode();
        if (encoded == 0) continue; // this recipe is empty.
        recipes.write((char*) &encoded, sizeof(encoded));
        counts += 1;
    }
    recipes.close();
    std::cout<<"we have write "<<counts<<" possible recipes."<<std::endl;
}

template<int N>
class SyncholizedHeapBitset {
    public:
    SyncholizedHeapBitset(void) {
        bitmap = new std::bitset<N>;
    }
    ~SyncholizedHeapBitset(void) {
        delete bitmap;
    }

    bool test(size_t pos) {return bitmap->test(pos);}
    void set(size_t pos, bool val) {bitmap->set(pos, val);}
    size_t count(void) {return bitmap->count();}

    private:
    std::bitset<N>* bitmap;
};

void searchBlock(
    std::mutex& lock,
    std::vector<Recipe>& recipes,// readonly
    SyncholizedHeapBitset<(2<<24)>& colormap, // thread-safe
    std::vector<Color>& oldColors, // read-only
    std::vector<Color>& newColors, //write-only
    std::fstream& searched, // write-only.
    size_t start, size_t end
) {
    for (size_t i = start; i < end; i++)
        {
            for(size_t j = 0; j < recipes.size(); j++) {
                Color crafted = recipes[j].getCraftedArmorColor(oldColors[i]);
                uint32_t encodedColor = crafted.encode();
                if (!colormap.test(encodedColor)) {
                    lock.lock(); // for thread safety.
                    if (!colormap.test(encodedColor)) { // double check after locked to avoid duplication.
                        colormap.set(encodedColor, true);
                        newColors.push_back(crafted);
                        uint32_t encodedOldColor = oldColors[i].encode();
                        searched.write((const char*) &encodedOldColor, sizeof(encodedOldColor));
                        uint64_t encodedRecipe = recipes[j].encode();
                        searched.write((const char*) &encodedRecipe, sizeof(encodedRecipe));
                        searched.write((const char*) &encodedColor, sizeof(encodedColor));
                    }
                    lock.unlock();
                }
            }
        }
}

void searchAllColors(const std::vector<Color>& colorlist, int numThreads=8, int blockSize=32) {
    std::cout<<"preparing..."<<std::endl;
    // generate all possible recipes for a leather.
    std::vector<Recipe> recipes;
    std::vector<std::vector<int>> ans;
    std::vector<int> current;
    std::cout<<"Generating compose..."<<std::endl;
    generateCompose(24, 16, current, ans);
    size_t counts = 0;
    std::cout<<"Converting compose to recipe..."<<std::endl;
    for (size_t i = 0; i < ans.size(); i++)
    {
        Recipe recipe(composeToRecipe(ans[i], 25));
        if (recipe.encode() == 0) continue; // this recipe is empty.
        recipes.push_back(recipe);
        counts += 1;
    }
    std::cout<<"we have find "<<counts<<" possible recipes."<<std::endl;
    // pre-craft all possible recipes to save time.
    std::cout<<"Pre-crafting recipe..."<<std::endl;
    for (size_t i = 0; i < recipes.size(); i++)
    {
        recipes[i].preCraft(colorlist);
    }

    // lets start to search.
    SyncholizedHeapBitset<(2<<24)> colormap;
    std::vector<Color> oldColors;
    // color #0x000000 do not exist in minecraft, so it is okay to be used as empty color.
    oldColors.push_back(Color(uint32_t(0)));
    std::mutex lock;

    size_t layer = 1;
    while (oldColors.size() > 0) {
        std::cout<<"processing layer "<<layer<<std::endl;
        std::fstream output;
        std::string savepath = std::string("layer") + std::to_string(layer) + std::string(".bin");
        std::cout<<"writing generated recipe in this layer to file "<<savepath<<"..."<<std::endl;
        output.open(
            savepath,
            std::fstream::out|std::fstream::binary
        );
        std::vector<Color> newColors;
        auto start = std::chrono::high_resolution_clock::now();
        auto last = start;
        for (size_t i = 0; i < oldColors.size(); i+=numThreads*blockSize)
        {   
            // create threads and dispatch jobs.
            std::vector<std::thread*> threads;
            for(int j = 0; j < numThreads; j++) {
                int start = i + j*blockSize;
                int end = i + (j+1)*blockSize;
                if (end > (int) oldColors.size()) {
                    end = (int) oldColors.size();
                }
                if (start > end) {start = end;}
                std::thread* t = new std::thread(
                    searchBlock, 
                    std::ref(lock),
                    std::ref(recipes),
                    std::ref(colormap),
                    std::ref(oldColors),
                    std::ref(newColors),
                    std::ref(output),
                    start, end
                );
                threads.push_back(t);
            }
            // wait for all threads to finish.
            for(size_t j = 0; j < threads.size(); j++) {
                threads[j]->join();
                delete threads[j];
                threads[j] = nullptr;
            }
            auto now = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> diff = now - start;
            std::chrono::duration<double> interval = now - last;
            if (interval.count() >= 5) {
                last = now;
                std::cout<<"progress: "<<double((i+1)*100)/oldColors.size()<<"%, ";
                std::cout<<colormap.count()<<" colors found.";
                double totalTime = diff.count()*oldColors.size()/(i+1);
                double remeaningTime = totalTime * (1 - (i+1)*1.0/oldColors.size());
                std::cout<<"estimating "<<totalTime<<" s in total and "<<remeaningTime<<" s left."<<std::endl;
            }
        }
        output.close();
        oldColors = newColors;
        std::cout<<"layer "<<layer<<" finished, found "<<oldColors.size()<<" new colors."<<std::endl;
        layer++;
    }
}

int main(int argc, char const *argv[])
{
    std::vector<Color> colors = {
        Color(176, 46, 38),
        Color(94, 124, 22),
        Color(137, 50, 184),
        Color(22, 156, 156),
        Color(157, 157, 151),
        Color(71, 79, 82),
        Color(243, 139, 170),
        Color(128, 199, 31),
        Color(254, 216, 61),
        Color(58, 179, 218),
        Color(199, 78, 189),
        Color(249, 128, 29),
        Color(29, 29, 33),
        Color(131, 84, 50),
        Color(60, 68, 170),
        Color(249, 255, 254),
    };

    // useless in this script, but useful for interpreting encoded recipe.
    std::vector<std::string> keys = {
        "red", "green", "purple", "cyan", 
        "silver", "gray", "pink", "lime", 
        "yellow", "lightBlue", "magenta", "orange", 
        "black", "brown", "blue", "white"
    }; 

    // parse numThreads.
    if (argc == 2) {
        int numThreads = std::stoi(std::string(argv[1]));
        searchAllColors(colors, numThreads);
    } else {
        std::cout<<"Usage: ./leatherColorCombinationsCalculator <numThreads>"<<std::endl;
    }
    
    return 0;
}