// window.h
#ifndef WINDOW_H
#define WINDOW_H

#ifdef __cplusplus
extern "C" {
#endif

void initialize_window(GLFWwindow* window);
void render_window(void);
void cleanup_window(void);

#ifdef __cplusplus
}
#endif

#endif // WINDOW_H
