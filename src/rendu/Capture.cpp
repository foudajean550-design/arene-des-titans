#include "Capture.hpp"

#include <cstdio>
#include <vector>

#include "GL.hpp"

namespace arene::rendu {

bool sauverPPM(const char* chemin, int largeur, int hauteur) {
    std::vector<unsigned char> px(static_cast<size_t>(largeur) * hauteur * 3);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, largeur, hauteur, GL_RGB, GL_UNSIGNED_BYTE, px.data());
    FILE* f = std::fopen(chemin, "wb");
    if (!f) return false;
    std::fprintf(f, "P6\n%d %d\n255\n", largeur, hauteur);
    for (int y = hauteur - 1; y >= 0; --y) std::fwrite(&px[static_cast<size_t>(y) * largeur * 3], 1, largeur * 3, f);
    std::fclose(f);
    return true;
}

}  // namespace arene::rendu
