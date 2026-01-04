#include <drogon/drogon.h>
int main() {
    // Set document root to serve static files (e.g., index.html)
    drogon::app().setDocumentRoot("./public");
    drogon::app().addListener("0.0.0.0", 8080);
    drogon::app().run();
    return 0;
}
