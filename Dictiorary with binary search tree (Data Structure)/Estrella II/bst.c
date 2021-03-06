/**************************************
* Authors: Morales, Marianela.        *
*          Rodriguez, Martin.         *
* Date of the last update: 05/05/2015 *
*                                     *
**************************************/

#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

#include "bst.h"
#include "data.h"
#include "index.h"
#include "list.h"
#include "pair.h"
#include "stack.h"

struct _tree_node_t {
    pair_t root;
    bst_t left;
    bst_t right;
};

bst_t bst_empty(void) {
    bst_t bst = NULL;
    return (bst);
}

bst_t bst_destroy(bst_t bst) {
    if (bst != NULL) {
        bst_t current = NULL;
        stack_t stack = stack_empty();
        stack = stack_push(stack, bst);

        while (!stack_is_empty(stack)) {
            current = stack_top(stack);
            stack = stack_pop(stack);
            current->root = pair_destroy(current->root);

            if (current->right != NULL) {
                stack = stack_push(stack, current->right);
                current->right = NULL;
            }
            if (current->left != NULL) {
                stack = stack_push(stack, current->left);
                current->left = NULL;
            }
            free(current);
            current = NULL;
        }
        stack = stack_destroy(stack);
        bst = NULL;
    }
    assert(bst == NULL);
    return (bst);
}

unsigned int bst_length(bst_t bst) {
    unsigned int length;
    length = 0;

    if (bst != NULL) {
        bst_t current = NULL;
        stack_t stack = stack_empty();
        stack = stack_push(stack, bst);

        while (!stack_is_empty(stack)) {
            current = stack_top(stack);
            length++;
            stack = stack_pop(stack);

            if (current->right != NULL) {
                stack = stack_push(stack, current->right);
            }
            if (current->left != NULL) {
                stack = stack_push(stack, current->left);
            }
            current = NULL;
        }
        stack = stack_destroy(stack);
    }
    return (length);
}

bool bst_is_equal(bst_t bst, bst_t other) {
    bool result = true;
    if (bst != NULL && other != NULL) {
        bst_t bstCurrent = bst, otherCurrent = other;
        stack_t stack = stack_empty();
        stack = stack_push(stack, bstCurrent);
        stack = stack_push(stack, otherCurrent);

        while (result && !stack_is_empty(stack)) {
            otherCurrent = stack_top(stack);
            stack = stack_pop(stack);
            bstCurrent = stack_top(stack);
            stack = stack_pop(stack);

            result = pair_is_equal(bstCurrent->root, otherCurrent->root);
            if (result) {
                if (bstCurrent->left != NULL && otherCurrent->left != NULL) {
                    stack = stack_push(stack, bstCurrent->left);
                    stack = stack_push(stack, otherCurrent->left);
                } else if (bstCurrent->left != NULL
                           || otherCurrent->left != NULL) {
                    result = false;
                }
            }
            if (result) {
                if (bstCurrent->right != NULL && otherCurrent->right != NULL) {
                    stack = stack_push(stack, bstCurrent->right);
                    stack = stack_push(stack, otherCurrent->right);
                } else if (bstCurrent->right != NULL
                           || otherCurrent->right != NULL) {
                    result = false;
                }
            }
            bstCurrent = NULL;
            otherCurrent = NULL;
        }
        stack = stack_destroy(stack);
    }
    return (result);
}

data_t bst_search(bst_t bst, index_t index) {
    bst_t current = bst;
    data_t data = NULL;
    while (current != NULL && data == NULL) {
        if (index_is_equal(index, pair_fst(current->root))) {
            data = pair_snd(current->root);
        } else {
            if (index_is_less_than(index, pair_fst(current->root))) {
                current = current->left;
            } else {
                current = current->right;
            }
        }
    }
    current = NULL;
    return (data);
}

bst_t bst_add(bst_t bst, index_t index, data_t data) {
    unsigned int prev_length = bst_length(bst);
    bst_t add = calloc(1, sizeof(struct _tree_node_t)), current = bst, prev;
    add->root = pair_from_index_data(index, data);
    add->left = NULL;
    add->right = NULL;

    if (bst != NULL) {
        while (current != NULL) {
            if (index_is_less_than(index, pair_fst(current->root))) {
                prev = current;
                current = current->left;
            } else {
                prev = current;
                current = current->right;
            }
        }
        if (index_is_less_than(index, pair_fst(prev->root))) {
            prev->left = add;
        } else {
            prev->right = add;
        }
        prev = NULL;
    } else {
        bst = add;
    }

    assert(prev_length + 1 == bst_length(bst));
    return bst;
}

pair_t bst_max(bst_t bst) {
    pair_t result = NULL;
    bst_t current = bst;

    while (current != NULL && result == NULL) {
        if (current->right == NULL) {
            result = pair_copy(current->root);
        } else {
            current = current->right;
        }
    }
    current = NULL;
    return (result);
}

bst_t delete_max(bst_t bst) {
    bst_t current = bst, tmp = NULL, prev = NULL;
    bool flag = false;
    while (current != NULL) {
        if (current->right == NULL) {
            tmp = current->left;
            /* Free the allocated resources */
            current->root = pair_destroy(current->root);
            free(current);
            current = NULL;
            if (flag) {
                prev->right = tmp;
            } else {
                bst = tmp;
            }
        } else {
            flag = true;
            prev = current;
            current = current->right;
        }
    }

    return (bst);
}

bst_t bst_remove(bst_t bst, index_t index) {
    unsigned int prev_length = bst_length(bst);
    bst_t current = bst, tmp = NULL, prev = bst;
    bool flag = true;
    if (current != NULL) {
        while (current != NULL) {
            if (index_is_less_than(index, pair_fst(current->root))) {
                flag = false;
                prev = current;
                current = current->left;
            } else if (index_is_equal(pair_fst(current->root), index) &&
                       current->left == NULL) {
                tmp = current->right;
                if (flag) {
                    bst = tmp;
                } else {
                    if (prev->left == current) {
                        prev->left = tmp;
                    } else {
                        prev->right = tmp;
                    }
                }
                current->root = pair_destroy(current->root);
                free(current);
                current = NULL;
            } else if (index_is_equal(pair_fst(current->root), index) &&
                       current->left != NULL) {
                current->root = pair_destroy(current->root);
                current->root = bst_max(current->left);
                current->left = delete_max(current->left);
                current = NULL;
            } else {
                flag = false;
                prev = current;
                current = current->right;
            }
        }
        prev = NULL;
        current = NULL;
        tmp = NULL;
    }
    assert(prev_length - 1 == bst_length(bst));
    return (bst);
}

bst_t bst_copy(bst_t bst) {
    bst_t result = bst_empty();

    if (bst != NULL) {
        bst_t current = bst;
        stack_t stack = stack_empty();

        index_t index = NULL;
        data_t data = NULL;

        while (!stack_is_empty(stack) || current != NULL) {
            if (current != NULL) {
                index = index_copy(pair_fst(current->root));
                data = data_copy(pair_snd(current->root));
                result = bst_add(result, index, data);
                if (current->right != NULL) {
                    stack = stack_push(stack, current->right);
                }
                current = current->left;
            } else {
                current = stack_top(stack);
                stack = stack_pop(stack);
            }

        }
        stack = stack_destroy(stack);
    }
    assert(bst_is_equal(result, bst));
    return result;
}

list_t bst_to_list(bst_t bst, list_t list) {

    if (bst != NULL) {
        bst_t current = bst;
        stack_t stack = stack_empty();

        index_t index = NULL;
        data_t data = NULL;

        while (!stack_is_empty(stack) || current != NULL) {
            if (current != NULL) {
                stack = stack_push(stack, current);
                current = current->left;
            } else {
                current = stack_top(stack);
                stack = stack_pop(stack);
                index = index_copy(pair_fst(current->root));
                data = data_copy(pair_snd(current->root));
                list = list_append(list, index, data);
                current = current->right;
            }
        }
        stack = stack_destroy(stack);
    }
    assert(bst_length(bst) == list_length(list));
    return (list);
}
