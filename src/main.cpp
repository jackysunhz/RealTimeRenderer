﻿// RealTimeRenderer.cpp : Defines the entry point for the application.
//

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "test_app.hpp"

using namespace std;

int main()
{
	vkrtr::TestApp app;
    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
