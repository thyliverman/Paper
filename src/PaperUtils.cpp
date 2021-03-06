#include "raylib.h"
#include <iostream>
#include <string>
#include <bits/stdc++.h>
#include "PaperApp.h"
#include "PaperUtils.h"
#include "PaperObjects.h"
#include <FreeImage.h>


Vector2 PaperUtils::CenterRectToPoint(Vector2 point, Vector2 size)
{
    return (Vector2){point.x - size.x / 2, point.y - size.y / 2};
}

Vector2 PaperUtils::CenterTextToPoint(Vector2 point, Font font, float fontSize, std::string text, float spacing = 0.0f)
{
    Vector2 textSize = MeasureTextEx(font, text.c_str(), fontSize, spacing);
    return (Vector2){point.x - textSize.x / 2, point.y - textSize.y / 2};
}

Vector2 PaperUtils::CenterTextToPoint(Vector2 point, FontData fontData, std::string text)
{
    return PaperUtils::CenterTextToPoint(point, fontData.font, fontData.fontSize, text, fontData.spacing);
}

Vector2 PaperUtils::CenterTextureToPoint(Vector2 point, Texture2D texture)
{
    return (Vector2){point.x - texture.width / 2, point.y - texture.height / 2};
}

// mouse detection methods
bool PaperUtils::IsMouseOver(Rectangle rect) {
    return CheckCollisionPointRec(GetMousePosition(), rect);
}

bool PaperUtils::IsMouseOver(Circle circle) {
    return CheckCollisionPointCircle(GetMousePosition(), circle.position, circle.radius);
}

bool PaperUtils::IsMouseOver(Texture2D texture, Vector2 pos) {
    Rectangle rec = (Rectangle){pos.x, pos.y, (float)texture.width, (float)texture.height};
    return CheckCollisionPointRec(GetMousePosition(), rec);
}

bool PaperUtils::RectClick(Rectangle rect, MouseButton mb) {
    return (IsMouseOver(rect) && IsMouseButtonDown(mb));
}
bool PaperUtils::RectClicked(Rectangle rect, MouseButton mb) {
    return (IsMouseOver(rect) && IsMouseButtonPressed(mb));
}

bool PaperUtils::TextureClick(Texture2D texture, Vector2 pos, MouseButton mb) {
    return (IsMouseOver(texture, pos) && IsMouseButtonDown(mb));
}
bool PaperUtils::TextureClicked(Texture2D texture, Vector2 pos, MouseButton mb) {
    return (IsMouseOver(texture, pos) && IsMouseButtonPressed(mb));
}

void PaperUtils::DrawCenteredObject(CenteredObject obj) {
    obj.Draw();
}

void PaperUtils::DrawCircle(Circle circle, Color color) {
    RAYLIB_H::DrawCircle(circle.position.x, circle.position.y, circle.radius, color);
}

void PaperUtils::DrawText(FontData fontData, std::string text, Vector2 position, Color color) {
    DrawTextEx(fontData.font, text.c_str(), position, fontData.fontSize, fontData.spacing, color);
}

bool PaperUtils::CompareColor(Color a, Color b) {
    if(a.r != b.r) {return false;}
    if(a.g != b.g) {return false;}
    if(a.b != b.b) {return false;}
    return true;
}

std::vector<Texture2D> PaperUtils::GetTexturesFromDirectory(std::string directory) {
    std::vector<std::string> extensions = {".png"};
    
    int count = 0;
    char **files = GetDirectoryFiles(directory.c_str(), &count);
    std::vector<std::string> paths;

    for(int i = 0; i < count; i++) {
        std::string path = directory + files[i];

        for (int x = 0; x < extensions.size(); x++) {
            if(IsFileExtension(path.c_str(), extensions[x].c_str()))
                paths.push_back(path);
        }
    }

    std::sort(paths.begin(), paths.end());
    std::vector<Texture2D> textures;

    for (int i = 0; i < paths.size(); i++) {
        textures.push_back(LoadTexture(paths[i].c_str()));
    }
    
    if(textures.size() < 1) {
        std::cout << "PAPER: WARNING: Failed to load any textures!" << std::endl;
    }

    return textures;
}

std::vector<Texture2D> PaperUtils::GetTexturesFromPaths(std::vector<std::string> paths) {
    std::vector<Texture2D> returnVector;
    std::cout << "PAPER: Parsing textures from a vector of paths";
    for(int i = 0; i < paths.size(); i++) {
        Texture2D tex = LoadTexture(paths[i].c_str());
        returnVector.push_back(tex);
    } 
    return returnVector;
}


std::vector<std::string> PaperUtils::LoadGIF(std::string path, PaperApp *instance) {
    
    std::vector<std::string> returnVector;

    FIMULTIBITMAP *bitmap = FreeImage_OpenMultiBitmap(FIF_GIF, path.c_str(), false, true);
    if(!bitmap) {
        std::cout << "PAPER: Failed to parse image file." << std::endl;
        return returnVector;
    }

    int frameCount = FreeImage_GetPageCount(bitmap);

    for(int i = 0; i < frameCount; i++) {
        std::string exportPath = ((std::string)"assets/" + GetFileNameWithoutExt(path.c_str()) +std::to_string(i) + (std::string)".png");
        FIBITMAP *frame = FreeImage_LockPage(bitmap, i);
        bool success = FreeImage_Save(FIF_PNG, frame, exportPath.c_str());
        FreeImage_UnlockPage(bitmap, frame, false);
    
        if(success) {
            returnVector.push_back(exportPath);
            instance->RemoveFileOnCleanup(exportPath);
        }
        else {
            std::cout << "PAPER: WARNING: Failed to process a frame in GIF processing";
        }
    }

    FreeImage_CloseMultiBitmap(bitmap);

    return returnVector;
}

void PaperUtils::ConvertToPng(std::string path, std::string outputPath, FREE_IMAGE_FORMAT currentFormat) {
    FIBITMAP *bitmap = FreeImage_Load(currentFormat, path.c_str(), BMP_DEFAULT);

    if(bitmap) {
        // loaded image successfully

        // attempt conversion to png
        if(FreeImage_Save(FIF_PNG, bitmap, outputPath.c_str(), PNG_DEFAULT)) {
            // on success
            std::cout << "PAPER: Successfully converted " + path + " to a PNG file format." << std::endl;
        }
        else {
            // on failure
            std::cout << "PAPER: Failed to convert " + path + " to a PNG file format." << std::endl;
        }

        FreeImage_Unload(bitmap);
    }
}

Rectangle PaperUtils::CreateRectangle(Vector2 p, Vector2 s) {
    return (Rectangle){p.x,p.y,s.x,s.y};
}
