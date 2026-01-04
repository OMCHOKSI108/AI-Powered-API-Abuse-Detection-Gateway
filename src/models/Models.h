#pragma once
#include <string>
#include <vector>

namespace models {

struct User {
    int id;
    std::string username;
    std::string email;
    std::string password_hash;
    std::string role; // "admin", "author", "reader"
    std::string bio;
};

struct Post {
    int id;
    int author_id;
    std::string title;
    std::string slug;
    std::string content;
    bool is_published;
    std::string created_at;
    std::vector<std::string> tags;
    int category_id;
    int views = 0;
    int likes = 0;
};

struct Comment {
    int id;
    int post_id;
    int user_id;
    std::string content;
    int likes = 0;
    bool is_reported = false;
};

struct Category {
    int id;
    std::string name;
};

}
