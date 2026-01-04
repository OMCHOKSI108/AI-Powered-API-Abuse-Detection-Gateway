#pragma once
#include <drogon/HttpController.h>
#include <vector>
#include <mutex>

struct Post {
    int id;
    std::string title;
    std::string content;
};

class BlogController : public drogon::HttpController<BlogController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(BlogController::getPosts, "/api/posts", drogon::Get);
        ADD_METHOD_TO(BlogController::createPost, "/api/posts", drogon::Post);
        ADD_METHOD_TO(BlogController::getPost, "/api/posts/{1}", drogon::Get);
    METHOD_LIST_END

    void getPosts(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void createPost(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void getPost(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id);

private:
    std::vector<Post> posts_;
    std::mutex mutex_;
    int nextId_ = 1;
};
