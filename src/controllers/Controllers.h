#pragma once
#include <drogon/HttpController.h>
#include "../models/Models.h"
#include <mutex>
#include <vector>

namespace api {

class AuthController : public drogon::HttpController<AuthController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(AuthController::registerUser, "/api/auth/register", drogon::Post);
        ADD_METHOD_TO(AuthController::login, "/api/auth/login", drogon::Post);
        ADD_METHOD_TO(AuthController::me, "/api/auth/me", drogon::Get);
    METHOD_LIST_END

    void registerUser(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void login(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void me(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};

class PostController : public drogon::HttpController<PostController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(PostController::getPosts, "/api/posts", drogon::Get);
        ADD_METHOD_TO(PostController::createPost, "/api/posts", drogon::Post);
        ADD_METHOD_TO(PostController::getPost, "/api/posts/{slug}", drogon::Get);
        ADD_METHOD_TO(PostController::updatePost, "/api/posts/{id}", drogon::Put);
        ADD_METHOD_TO(PostController::deletePost, "/api/posts/{id}", drogon::Delete);
        ADD_METHOD_TO(PostController::publish, "/api/posts/{id}/publish", drogon::Post);
    METHOD_LIST_END

    void getPosts(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void createPost(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void getPost(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, std::string slug);
    void updatePost(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id);
    void deletePost(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id);
    void publish(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id);
};

class CategoryController : public drogon::HttpController<CategoryController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(CategoryController::getCategories, "/api/categories", drogon::Get);
        ADD_METHOD_TO(CategoryController::createCategory, "/api/categories", drogon::Post);
    METHOD_LIST_END
    void getCategories(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void createCategory(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};

class CommentController : public drogon::HttpController<CommentController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(CommentController::getComments, "/api/posts/{id}/comments", drogon::Get);
        ADD_METHOD_TO(CommentController::createComment, "/api/posts/{id}/comments", drogon::Post);
    METHOD_LIST_END
    void getComments(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id);
    void createComment(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id);
};

class UserController : public drogon::HttpController<UserController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(UserController::getUser, "/api/users/{id}", drogon::Get);
        ADD_METHOD_TO(UserController::updateUser, "/api/users/{id}", drogon::Put);
    METHOD_LIST_END
    void getUser(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id);
    void updateUser(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id);
};

// Global Store (Mock Database)
class DataStore {
public:
    static DataStore& get() {
        static DataStore instance;
        return instance;
    }
    
    std::vector<models::User> users;
    std::vector<models::Post> posts;
    std::vector<models::Category> categories;
    std::vector<models::Comment> comments;
    
    std::mutex mutex;
    int nextUserId = 1;
    int nextPostId = 1;
    int nextCategoryId = 1;
    int nextCommentId = 1;
};

}
