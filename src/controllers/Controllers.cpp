#include "Controllers.h"
#include <drogon/utils/Utilities.h>

namespace api {

// --- AuthController ---

void AuthController::registerUser(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    auto json = req->getJsonObject();
    if (!json) {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k400BadRequest);
        callback(resp);
        return;
    }
    
    std::string email = (*json)["email"].asString();
    std::string password = (*json)["password"].asString(); // In real app, hash this!
    
    std::lock_guard<std::mutex> lock(DataStore::get().mutex);
    DataStore::get().users.push_back({DataStore::get().nextUserId++, "user", email, password, "author", ""});
    
    Json::Value ret;
    ret["status"] = "registered";
    ret["userId"] = DataStore::get().nextUserId - 1;
    callback(drogon::HttpResponse::newHttpJsonResponse(ret));
}

void AuthController::login(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    // Mock login - returns a fake token
    Json::Value ret;
    ret["token"] = "fake-jwt-token-123";
    callback(drogon::HttpResponse::newHttpJsonResponse(ret));
}

void AuthController::me(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    Json::Value ret;
    ret["id"] = 1;
    ret["role"] = "admin";
    callback(drogon::HttpResponse::newHttpJsonResponse(ret));
}

// --- PostController ---

void PostController::getPosts(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    std::lock_guard<std::mutex> lock(DataStore::get().mutex);
    Json::Value arr(Json::arrayValue);
    for(const auto& p : DataStore::get().posts) {
        if(p.is_published) {
            Json::Value v;
            v["id"] = p.id;
            v["title"] = p.title;
            v["slug"] = p.slug;
            arr.append(v);
        }
    }
    Json::Value ret;
    ret["posts"] = arr;
    callback(drogon::HttpResponse::newHttpJsonResponse(ret));
}

void PostController::createPost(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    auto json = req->getJsonObject();
    std::lock_guard<std::mutex> lock(DataStore::get().mutex);
    
    models::Post p;
    p.id = DataStore::get().nextPostId++;
    p.title = (*json)["title"].asString();
    p.content = (*json)["content"].asString();
    
    // Manual slug generation
    p.slug = p.title;
    std::transform(p.slug.begin(), p.slug.end(), p.slug.begin(), 
                   [](unsigned char c){ return std::tolower(c); });
    // basic slugify: replace space with -
    std::replace(p.slug.begin(), p.slug.end(), ' ', '-');
    
    p.is_published = false; // Draft by default
    
    DataStore::get().posts.push_back(p);
    
    Json::Value ret;
    ret["id"] = p.id;
    ret["slug"] = p.slug;
    callback(drogon::HttpResponse::newHttpJsonResponse(ret));
}

void PostController::getPost(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, std::string slug) {
    std::lock_guard<std::mutex> lock(DataStore::get().mutex);
    for(const auto& p : DataStore::get().posts) {
        if(p.slug == slug) {
            Json::Value v;
            v["id"] = p.id;
            v["title"] = p.title;
            v["content"] = p.content;
            v["views"] = p.views;
            callback(drogon::HttpResponse::newHttpJsonResponse(v));
            return;
        }
    }
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k404NotFound);
    callback(resp);
}

void PostController::updatePost(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id) {
    auto json = req->getJsonObject();
    if (!json) {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k400BadRequest);
        callback(resp);
        return;
    }
    
    std::lock_guard<std::mutex> lock(DataStore::get().mutex);
    for(auto& p : DataStore::get().posts) {
        if(p.id == id) {
            if((*json).isMember("title")) p.title = (*json)["title"].asString();
            if((*json).isMember("content")) p.content = (*json)["content"].asString();
            if((*json).isMember("tags")) {
                p.tags.clear();
                for(const auto& t : (*json)["tags"]) p.tags.push_back(t.asString());
            }
            
            Json::Value ret;
            ret["status"] = "updated";
            ret["id"] = p.id;
            callback(drogon::HttpResponse::newHttpJsonResponse(ret));
            return;
        }
    }
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k404NotFound);
    callback(resp);
}

void PostController::deletePost(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id) {
    std::lock_guard<std::mutex> lock(DataStore::get().mutex);
    auto& posts = DataStore::get().posts;
    auto it = std::remove_if(posts.begin(), posts.end(), [id](const models::Post& p){ return p.id == id; });
    if(it != posts.end()) {
        posts.erase(it, posts.end());
        Json::Value ret;
        ret["status"] = "deleted";
        callback(drogon::HttpResponse::newHttpJsonResponse(ret));
    } else {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k404NotFound);
        callback(resp);
    }
}

void PostController::publish(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id) {
    std::lock_guard<std::mutex> lock(DataStore::get().mutex);
    for(auto& p : DataStore::get().posts) {
        if(p.id == id) {
            p.is_published = true;
            Json::Value ret;
            ret["status"] = "published";
            callback(drogon::HttpResponse::newHttpJsonResponse(ret));
            return;
        }
    }
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k404NotFound);
    callback(resp);
}

// --- CategoryController ---

void CategoryController::getCategories(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    std::lock_guard<std::mutex> lock(DataStore::get().mutex);
    Json::Value arr(Json::arrayValue);
    for(const auto& c : DataStore::get().categories) {
        Json::Value item;
        item["id"] = c.id;
        item["name"] = c.name;
        arr.append(item);
    }
    Json::Value ret;
    ret["categories"] = arr;
    callback(drogon::HttpResponse::newHttpJsonResponse(ret));
}

void CategoryController::createCategory(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    auto json = req->getJsonObject();
    if(!json || !(*json).isMember("name")) {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k400BadRequest);
        callback(resp);
        return;
    }
    
    std::lock_guard<std::mutex> lock(DataStore::get().mutex);
    models::Category c;
    c.id = DataStore::get().nextCategoryId++;
    c.name = (*json)["name"].asString();
    DataStore::get().categories.push_back(c);
    
    Json::Value ret;
    ret["id"] = c.id;
    ret["name"] = c.name;
    callback(drogon::HttpResponse::newHttpJsonResponse(ret));
}

// --- CommentController ---

void CommentController::getComments(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id) {
    std::lock_guard<std::mutex> lock(DataStore::get().mutex);
    Json::Value arr(Json::arrayValue);
    for(const auto& c : DataStore::get().comments) {
        if(c.post_id == id) {
            Json::Value item;
            item["id"] = c.id;
            item["user_id"] = c.user_id;
            item["content"] = c.content;
            arr.append(item);
        }
    }
    Json::Value ret;
    ret["comments"] = arr;
    callback(drogon::HttpResponse::newHttpJsonResponse(ret));
}

void CommentController::createComment(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id) {
    auto json = req->getJsonObject();
    if(!json || !(*json).isMember("content")) {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k400BadRequest);
        callback(resp);
        return;
    }
    
    std::lock_guard<std::mutex> lock(DataStore::get().mutex);
    models::Comment c;
    c.id = DataStore::get().nextCommentId++;
    c.post_id = id;
    c.user_id = 1; // Correctly mocked user ID
    c.content = (*json)["content"].asString();
    DataStore::get().comments.push_back(c);
    
    Json::Value ret;
    ret["id"] = c.id;
    ret["post_id"] = id;
    callback(drogon::HttpResponse::newHttpJsonResponse(ret));
}

// --- UserController ---

void UserController::getUser(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id) {
    std::lock_guard<std::mutex> lock(DataStore::get().mutex);
    for(const auto& u : DataStore::get().users) {
        if(u.id == id) {
            Json::Value ret;
            ret["id"] = u.id;
            ret["username"] = u.username;
            ret["role"] = u.role;
            ret["bio"] = u.bio;
            callback(drogon::HttpResponse::newHttpJsonResponse(ret));
            return;
        }
    }
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k404NotFound);
    callback(resp);
}

void UserController::updateUser(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id) {
     auto json = req->getJsonObject();
    std::lock_guard<std::mutex> lock(DataStore::get().mutex);
    for(auto& u : DataStore::get().users) {
        if(u.id == id) {
            if(json && (*json).isMember("bio")) u.bio = (*json)["bio"].asString();
            Json::Value ret;
            ret["status"] = "updated";
            callback(drogon::HttpResponse::newHttpJsonResponse(ret));
            return;
        }
    }
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k404NotFound);
    callback(resp);
}

}
