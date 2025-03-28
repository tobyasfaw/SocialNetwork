#include <iostream>
#include <string>
#include "post.h"

// Base Post class methods

Post::Post() : messageId_(0), ownerId_(0), likes_(0), privacyLevel(PUBLIC) {}

Post::Post(int messageId, int ownerID, std::string message, int likes, Post::PrivacyLevel level)
    : messageId_(messageId), ownerId_(ownerID), message_(message), likes_(likes), privacyLevel(level) {}

std::string Post::toString() {
    return message_ + " liked by " + std::to_string(likes_) + " people";
}

int Post::getMessageId() const {
    return messageId_;
}

int Post::getOwnerId() const {
    return ownerId_;
}

std::string Post::getMessage() const {
    return message_;
}

int Post::getLikes() const {
    return likes_;
}

Post::PrivacyLevel Post::getPrivacyLevel() const {
    return privacyLevel;
}

void Post::setPrivacyLevel(Post::PrivacyLevel level) {
    privacyLevel = level;
}

bool Post::getIsPublic() const {
    return privacyLevel == PUBLIC;
}

std::string Post::getAuthor() const {
    return "";
}

// IncomingPost class methods

IncomingPost::IncomingPost() : Post(), author_("Unknown") {}

IncomingPost::IncomingPost(int messageId, int ownerId, std::string message, int likes, Post::PrivacyLevel level, std::string author)
    : Post(messageId, ownerId, message, likes), author_(author) {
    setPrivacyLevel(level); // Set privacy using the base class method
}



std::string IncomingPost::toString() {
    if (getPrivacyLevel() == Post::PrivacyLevel::PUBLIC) {
        return author_ + " wrote: " + getMessage() + " liked by " + std::to_string(getLikes()) + " people";
    } else if (getPrivacyLevel() == Post::PrivacyLevel::PRIVATE) {
        return author_ + " wrote (private): [Content Hidden] liked by " + std::to_string(getLikes()) + " people";
    } else {
        return author_ + " wrote (semi-private): " + getMessage() + " liked by " + std::to_string(getLikes()) + " people";
    }
}


std::string IncomingPost::getAuthor() const {
    return author_;
}
