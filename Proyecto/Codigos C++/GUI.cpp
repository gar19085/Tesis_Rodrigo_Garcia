#include <iostream>

int main() {
    // Load the HTML template
    std::string htmlTemplate = loadTemplate("GUIC.html");

    // Replace placeholder with C++ value
    std::string finalHtml = replacePlaceholder(htmlTemplate, "cpp_value", "42");

    // Send the final HTML to the client
    std::cout << "Content-type:text/html\r\n\r\n";
    std::cout << finalHtml;

    return 0;
}
