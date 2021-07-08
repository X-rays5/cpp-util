//
// Created by X-ray on 7/8/2021.
//
#pragma once
#include <memory>
#include <string>

struct LinkedListNode {
    std::string value;
    std::shared_ptr<LinkedListNode> next = nullptr;
    explicit LinkedListNode(std::string init_value) : value(std::move(init_value))
    {
    }
};

class CLinkedList {
public:
    CLinkedList() : head(std::make_shared<LinkedListNode>(""))
    {
    }

    // add item to back of the list
    LinkedListNode append(const std::string& value) {
        std::shared_ptr<LinkedListNode> curnode = GetHead();
        while (curnode->next != nullptr) {
            curnode = curnode->next;
        }
        std::shared_ptr<LinkedListNode> node = std::make_unique<LinkedListNode>(value);
        curnode->next = node;
        return *node;
    }

    // removed last item in the list
    void pop() {
        std::shared_ptr<LinkedListNode> previous = nullptr;
        std::shared_ptr<LinkedListNode> curnode = GetHead();
        while (curnode) {
            previous = curnode;
            curnode = curnode->next;
        }
        previous->next = nullptr;
    }

    // gives first node
    std::shared_ptr<LinkedListNode> GetHead() {
        return head;
    }
private:
    std::shared_ptr<LinkedListNode> head;
};