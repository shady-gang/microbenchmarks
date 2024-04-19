template<typename K>
struct TreeNode {
    TreeNode* children[2];
    int depth;
    K key;
};

template<typename K>
FN_PREFIX TreeNode<K>* find(TreeNode<K>* tree, K value) {
    while (tree) {
        if (tree->key == value)
            return tree;
        else if (value < tree->key && tree->children[0])
            tree = tree->children[0];
        else if (value > tree->key && tree->children[1])
            tree = tree->children[1];
    }
    return nullptr;
}

template<typename K>
FN_PREFIX TreeNode<K>* find_rec(TreeNode<K>* tree, K value) {
    if (tree->key == value)
        return tree;
    else if (value < tree->key && tree->children[0])
        return find(tree->children[0], value);
    else if (value > tree->key && tree->children[1])
        return find(tree->children[1], value);
    return nullptr;
}

template<typename K>
FN_PREFIX TreeNode<K>* skew(TreeNode<K>* t) {
    if (t->children[0] == nullptr)
        return t;
    else if (t->children[0]->depth == t->depth) {
        auto l = t->children[0];
        t->children[0] = l->children[1];
        l->children[1] = t;
        return l;
    } else
        return t;
}

template<typename K>
FN_PREFIX TreeNode<K>* split(TreeNode<K>* t) {
    if (!t->children[1] || !t->children[1]->children[1])
        return t;
    else if (t->depth == t->children[1]->children[1]->depth) {
        auto r = t->children[1];
        t->children[1] = r->children[0];
        r->children[0] = t;
        r->depth += 1;
        return r;
    } else
        return t;
}

template<typename K>
FN_PREFIX TreeNode<K>* insert(TreeNode<K>* t, TreeNode<K>* x) {
    if (t == nullptr) {
        x->depth = 1;
        return x;
    } else if (x->key < t->key) {
        t->children[0] = insert(t->children[0], x);
    } else if (x->key > t->key) {
        t->children[1] = insert(t->children[1], x);
    }

    t = skew(t);
    t = split(t);

    return t;
}