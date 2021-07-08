//
// Created by X-ray on 7/8/2021.
//
#pragma once
#include <memory>
#include <string>

struct DoubleLinkedListNode {
    std::string value;
    std::shared_ptr<DoubleLinkedListNode> next = nullptr;
    std::shared_ptr<DoubleLinkedListNode> previous = nullptr;
    explicit DoubleLinkedListNode(std::string init_value) : value(std::move(init_value))
    {
    }
};

class CDoubleLinkedList {
public:
    CDoubleLinkedList() : head(std::make_shared<DoubleLinkedListNode>(""))
    {
    }

    // add item to back of the list
    DoubleLinkedListNode append(const std::string& value) {
        std::shared_ptr<DoubleLinkedListNode> curnode = GetHead();
        while (curnode->next != nullptr) {
            curnode = curnode->next;
        }
        std::shared_ptr<DoubleLinkedListNode> node = std::make_unique<DoubleLinkedListNode>(value);
        curnode->next = node;
        curnode->next->previous = curnode;
        return *node;
    }

    // removed last item in the list
    void pop() {
        std::shared_ptr<DoubleLinkedListNode> previous = nullptr;
        std::shared_ptr<DoubleLinkedListNode> curnode = GetHead();
        while (curnode) {
            previous = curnode;
            curnode = curnode->next;
        }
        previous->next = nullptr;
    }

    // gives first node
    std::shared_ptr<DoubleLinkedListNode> GetHead() {
        return head;
    }
private:
    std::shared_ptr<DoubleLinkedListNode> head;
};
