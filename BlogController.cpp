#include "BlogController.h"

void BlogController::getPosts(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    Json::Value ret;
    Json::Value arr(Json::arrayValue);
    
    for (const auto& p : posts_) {
        Json::Value item;
        item["id"] = p.id;
        item["title"] = p.title;
        item["content"] = p.content;
        arr.append(item);
    }
    ret["posts"] = arr;
    auto resp = drogon::HttpResponse::newHttpJsonResponse(ret);
    callback(resp);
}

void BlogController::createPost(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto json = req->getJsonObject();
    if (!json || !(*json).isMember("title") || !(*json).isMember("content")) {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k400BadRequest);
        resp->setBody("Missing title or content");
        callback(resp);
        return;
    }
    
    Post p;
    p.id = nextId_++;
    p.title = (*json)["title"].asString();
    p.content = (*json)["content"].asString();
    posts_.push_back(p);
    
    Json::Value ret;
    ret["id"] = p.id;
    ret["status"] = "created";
    auto resp = drogon::HttpResponse::newHttpJsonResponse(ret);
    resp->setStatusCode(drogon::k201Created);
    callback(resp);
}

void BlogController::getPost(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& p : posts_) {
        if (p.id == id) {
            Json::Value item;
            item["id"] = p.id;
            item["title"] = p.title;
            item["content"] = p.content;
            auto resp = drogon::HttpResponse::newHttpJsonResponse(item);
            callback(resp);
            return;
        }
    }
    
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k404NotFound);
    callback(resp);
}
