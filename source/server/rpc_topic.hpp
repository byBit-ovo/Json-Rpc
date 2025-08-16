#pragma once
#include "../network/network.hpp"
#include "../network/message.hpp"
#include <set>
#include <unordered_set>

namespace MyRpc
{
    namespace Server
    {
        class TopicManager{
            public:
                using ptr = std::shared_ptr<TopicManager>;
                //To dispatcher
                void onTopicRequest(const ConnectionBase::ptr &conn, const TopicRequest::ptr &msg){
                    //create、delete、subscribe、cancel、publish

                }
                void onShutDown(const ConnectionBase::ptr &conn); 
            private:
                void removeTopic(const std::string &topic_name){

                    std::vector<Subscriber::ptr> pending_remove;
                    {
                        std::lock_guard<std::mutex> guard(_mutex);
                        auto iter = _topics.find(topic_name);
                        if(iter==_topics.end()){
                            return;
                        }
                        Topic::ptr topic = iter->second;
                        std::unordered_set<ConnectionBase::ptr> subscribers = topic->_subscribers;
                        for(const ConnectionBase::ptr &conn: subscribers){
                            auto it = _subscribers.find(conn);
                            if(it == _subscribers.end()){
                                continue;
                            }
                            Subscriber::ptr subscriber = it->second;
                            pending_remove.push_back(std::move(subscriber));
                        }
                        _topics.erase(topic_name);
                    }
                    // Prevent two unrelated locks from affecting each other and causing performance degration
                    // so remove here, 
                    for(auto &subs: pending_remove){
                        subs->unsubscribe(topic_name);
                    }

                }
                void createTopic(const ConnectionBase::ptr &conn,const TopicRequest::ptr &msg){
                    std::lock_guard<std::mutex> guard(_mutex);
                    Topic::ptr topic = std::make_shared<Topic>(msg->topicKey());
                    _topics.insert(std::make_pair(msg->topicKey(),topic));
                }

                struct Subscriber{
                    using ptr = std::shared_ptr<Subscriber>;
                    ConnectionBase::ptr conn;
                    std::mutex mutex;
                    std::unordered_set<std::string> topics;           //topics subscribed by this people
                    Subscriber(const ConnectionBase::ptr &con):conn(con){}

                    //call when this person subscribe sometopic
                    void subscribe(const std::string &topic_name){
                        std::unique_lock<std::mutex> guard(mutex);
                        topics.insert(topic_name);
                    }
                    //call when sometopic is deleted or this person unsubscribe
                    void unsubscribe(const std::string &topic_name){
                        std::unique_lock<std::mutex> guard(mutex);
                        topics.erase(topic_name);
                    }
                };

                struct Topic{
                    using ptr = std::shared_ptr<Topic>;
                    std::string topic_name;
                    std::mutex mutex;
                    std::unordered_set<ConnectionBase::ptr> _subscribers; //subscribers of this topic
                    Topic(const std::string &name):topic_name(name){}
                    //call when someone subscribe this topic
                    void appendSubscriber(const ConnectionBase::ptr &conn){
                        std::unique_lock<std::mutex> guard(mutex);
                        _subscribers.insert(conn);
                    }
                    //call when someone unsubscribe this topic
                    void removeSubscriber(const ConnectionBase::ptr &conn){
                        std::unique_lock<std::mutex> guard(mutex);
                        _subscribers.erase(conn);
                    }
                    //call when message push to this topic
                    void pushMessage(const MessageBase::ptr &msg){
                        std::unique_lock<std::mutex> guard(mutex);
                        for(const auto &subscriber : _subscribers){
                            subscriber->send(msg);
                        }
                    }
                };

            private:
                std::mutex _mutex;
                //conn: subs
                std::unordered_map<std::string, Topic::ptr> _topics;
                std::unordered_map<ConnectionBase::ptr, Subscriber::ptr> _subscribers;

        };
    }
}