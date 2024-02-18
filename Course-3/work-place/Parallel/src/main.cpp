#include "image_editor.hpp"

int main(int argc, char* argv[]) 
{   
    ImageEditor editor;
    editor.editImage(argv[1], argv[2]);
    return 0;
}
