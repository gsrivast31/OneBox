//
//  AppColors.h
//  OneDrive
//
//  Created by Ajay on 5/24/13.
//  Copyright (c) 2013 OneDrive. All rights reserved.
//

#ifndef OneDrive_AppColors_h
#define OneDrive_AppColors_h

#define FONT UIFont
// Contains methods for all the App color/background image values.
// We should try to place the code to access all these type of values here.

namespace AppplicationUI {
    
    struct ColorValues
    {
        float r;
        float g;
        float b;
        float a;
    };
    
    
    void getInterfaceColor(ColorValues &val);
    void getInterfaceTextColor(ColorValues &val);
    void getBlockInterfaceColor(ColorValues &val);
    void getBlockTextColor(ColorValues &val);
    void getHeadingTextColor(ColorValues &val);
    void getHeadingFontSize(int &size);
    void getInterfaceFontSize(int &size);
    void getBlockTextSize();

    void getDetailsFont(FONT &fnt);
    void getHeadingFont(FONT &fnt);
    void getBlocksFont(FONT &fnt);
    
    // Add more... for the grid.... and for the dialogs like Settings etc.
    
    
}


#endif
