#ifndef POST_H
#define POST_H
#include <string>

class Post {
public:
    enum PrivacyLevel { PUBLIC, PRIVATE,};

    // Constructors and destructor
    Post();
    virtual ~Post() = default;
    Post(int messageId, int ownerID, std::string message, int likes, PrivacyLevel level = PUBLIC);

    // Virtual methods
    virtual std::string toString();
    virtual int getMessageId() const;
    virtual int getOwnerId() const;
    virtual std::string getMessage() const;
    virtual int getLikes() const;
    virtual std::string getAuthor() const;
    virtual bool getIsPublic() const;

    // Privacy methods
    PrivacyLevel getPrivacyLevel() const;
    void setPrivacyLevel(PrivacyLevel level);

protected:
    int messageId_;
    int ownerId_;
    std::string message_;
    int likes_;
    PrivacyLevel privacyLevel;
};

class IncomingPost : public Post {
public:
    IncomingPost();
    IncomingPost(int messageId, int ownerId, std::string message, int likes, Post::PrivacyLevel level, std::string author);
    std::string toString() override;
    std::string getAuthor() const override;

private:
    std::string author_;
};


#endif
