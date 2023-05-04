#pragma once
#include <vector>

template < class T > class SimpleTreeNode
{
public:
	SimpleTreeNode() {

	}

	SimpleTreeNode(const T& t) {
		this->t = t;
	}

	virtual ~SimpleTreeNode() {

	}

	SimpleTreeNode* addChild(const T& t) {
		this->children.push_back(SimpleTreeNode(t));
		return &(* (this->children.end() - 1));
	}

	void deleteChild(const T& t) {
		for (int i = 0; i < this->children.size(); i++) {
			if (this->children.at(i).t == t) {
				this->children.erase(this->children.begin() + i);
				return;
			}
		}
	}

	void deleteChildByIndex(const int ind) {
		this->children.erase(this->children.begin() + ind);
	}

	void set(const T& t) {
		this->t = t;
	}

	T& get() {
		return this->t;
	}

	const T& get() const {
		return this->t;
	}

	std::vector<SimpleTreeNode>& getChildren() {
		return this->children;
	}

	const std::vector<SimpleTreeNode>& getChildren() const {
		return this->children;
	}
	
	void searchSubtreeByIndex(const T& t, T** output) {
		if (this->children.size() > 0) {
			for (int i = 0; i < this->children.size(); i++) {
				if (this->children.at(i).t == t) {
					*output = &(this->children.at(i).t);
					return;
				}
				this->children.at(i).searchSubtreeByIndex(t, output);
			}
		}
		else {
			return;
		}
		
	}

private:
	T t;
	std::vector<SimpleTreeNode> children;
};

