#include "window.h"


void Window::draw() {}

void Window::operator()() {
    if (shouldBringToFront) {
        ImGui::SetNextWindowFocus();
        shouldBringToFront = false;
    }

    draw();
}
