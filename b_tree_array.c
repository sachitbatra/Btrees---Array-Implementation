#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define degree 6

struct data
{
    char *string1;
    char *string2;
    int number1;
    int number2;
};

struct tree_node
{
    int *keys;
    int t;
    int *children;
    int num_keys;
    bool leaf;
    struct data *values;
};

struct tree_node **b_tree_array;
int next_free_index = 0;

int search(int, int);
void traverse(int);
int create_node(int, bool);
void insert(int, int*, struct data);
void split_child(int, int, int);
void insert_non_full(int, int, struct data);
int findKey(int, int);
void remove_key(int, int);
void remove_from_leaf(int, int);
void remove_from_non_leaf(int, int);
int get_pred(int, int);
struct data get_pred_data(int, int);
int get_succ(int, int);
struct data get_succ_data(int, int);
int fill(int, int);
void borrow_from_prev(int, int);
void borrow_from_next(int, int);
void merge(int, int);

int traverse_count = 0;

int main()
{
    b_tree_array = (struct tree_node**)malloc(10000000 * sizeof(struct tree_node*));

    int root = 0;

    FILE *fp;
    fp = fopen("dataset.csv", "r");

    if (fp == NULL)
    {
        printf("Error in opening the file.");
    }

    int cur_key;
    rewind(fp);

    for (int i=0; i<1000000; i++)
    {
        struct data cur_element;
        cur_element.string1 = (char*)malloc(sizeof(char) * 10);
        cur_element.string2 = (char*)malloc(sizeof(char) * 10);

        fscanf(fp, "%d,%[^,],%[^,],%d,%d", &cur_key, cur_element.string1, cur_element.string2, &cur_element.number1, &cur_element.number2);

        insert(cur_key, &root, cur_element);
    }

    int user_input;
    while(1)
    {
        printf("1. Search for an Entry.\n2. Delete an Entry.\n3. Exit.\n");
        scanf("%d", &user_input);

        if (user_input == 1)
        {
            int key_value;
            printf("Enter the Key of the Entry you want to Search for: ");
            scanf("%d", &key_value);
            search(root, key_value);
        }

        else if (user_input == 2)
        {
            int key_value;
            printf("Enter the Key of the Entry you want to Delete: ");
            scanf("%d", &key_value);
            printf("Deleting: ");
            search(root, key_value);
            remove_key(root, key_value);
        }

        else if (user_input == 3)
        {
            break;
        }
    }
}

void traverse(int root_index)
{
    int index = 0;

    for (; index<(b_tree_array[root_index])->num_keys; index++)
    {
        if ((b_tree_array[root_index])->leaf == false)
        {
            traverse((b_tree_array[root_index])->children[index]);
        }

        //printf("%d\n", (b_tree_array[root_index])->keys[index]);
        traverse_count++;
    }

    if ((b_tree_array[root_index])->leaf == false)
    {
        traverse((b_tree_array[root_index])->children[index]);
    }
}

int create_node (int t, bool is_leaf)
{
    struct tree_node* new_node;
    new_node = (struct tree_node*)malloc(sizeof(struct tree_node));
    new_node->t = t;
    new_node->leaf = is_leaf;
    new_node->num_keys = 0;

    new_node->keys = (int*)malloc(sizeof(int) * ((2*t) - 1));
    new_node->children = (int*)malloc(sizeof(int) * (2*t));
    new_node->values = (struct data*)malloc(sizeof(struct data) * ((2*t) - 1));

    b_tree_array[next_free_index++] = new_node;

    return (next_free_index - 1);
}

int search(int root_index, int key)
{
    int index = 0;

    while ((index < ((b_tree_array[root_index])->num_keys)) && ((key > (((b_tree_array[root_index])->keys)[index]))))
    {
        index++;
    }

    if ((((b_tree_array[root_index])->keys)[index]) == key)
    {
        struct tree_node* found_node = (b_tree_array[root_index]);
        printf("%s, %s, %d, %d. \n", (found_node->values)[index].string1, (found_node->values)[index].string2, (found_node->values)[index].number1, (found_node->values)[index].number2);
        return 1;
    }

    if ((b_tree_array[root_index])->leaf == true)
    {
        printf("Not Found.\n");
        return -1;
    }

    else
    {
        return (search((((b_tree_array[root_index])->children)[index]), key));
    }
}

void insert(int key, int *root_index, struct data key_data)
{
    if (b_tree_array[*root_index] == NULL)
    {
        *root_index = create_node(degree, true);
        ((b_tree_array[*root_index])->keys)[0] = key;
        ((b_tree_array[*root_index])->values)[0] = key_data;
        (b_tree_array[*root_index])->num_keys = 1;
    }

    else
    {
        if ((b_tree_array[*root_index])->num_keys == ((2 * degree) - 1))
        {
            int new_root = create_node(degree, false);
            ((b_tree_array[new_root])->children)[0] = (*root_index);

            split_child(0, new_root, (*root_index));

            int index = 0;
            if (((b_tree_array[new_root])->keys)[0] < key)
            {
                index++;
            }
            insert_non_full(key, (((b_tree_array[new_root])->children)[index]), key_data);

            *root_index = new_root;
        }

        else
        {
            insert_non_full(key, (*root_index), key_data);
        }
    }
}

void split_child(int index, int parent, int full_child)
{
    int new_child_index = create_node((b_tree_array[full_child])->t, (b_tree_array[full_child])->leaf);
    (b_tree_array[new_child_index])->num_keys = degree-1;

    for (int key_index=0; key_index < (degree - 1); key_index++)
    {
        ((b_tree_array[new_child_index])->keys)[key_index] = ((b_tree_array[full_child])->keys)[key_index + degree];
        ((b_tree_array[new_child_index])->values)[key_index] = ((b_tree_array[full_child])->values)[key_index + degree];
    }

    if ((b_tree_array[full_child])->leaf == false)
    {
        for (int key_index=0; key_index < degree; key_index++)
        {
            ((b_tree_array[new_child_index])->children)[key_index] = ((b_tree_array[full_child])->children)[key_index + degree];
        }
    }

    (b_tree_array[full_child])->num_keys = degree - 1;

    for (int key_index=(b_tree_array[parent])->num_keys; key_index >= (index+1); key_index--)
    {
        (b_tree_array[parent])->children[key_index+1] = (b_tree_array[parent])->children[key_index];
    }

    (b_tree_array[parent])->children[index+1] = new_child_index;

    for (int key_index=((b_tree_array[parent])->num_keys) - 1; key_index >= index; key_index--)
    {
        (b_tree_array[parent])->keys[key_index+1] = (b_tree_array[parent])->keys[key_index];
        (b_tree_array[parent])->values[key_index+1] = (b_tree_array[parent])->values[key_index];
    }

    (b_tree_array[parent])->keys[index] = (b_tree_array[full_child])->keys[degree - 1];
    (b_tree_array[parent])->values[index] = (b_tree_array[full_child])->values[degree - 1];

    (b_tree_array[parent])->num_keys = (b_tree_array[parent])->num_keys + 1;
}

void insert_non_full(int key, int node_index, struct data key_data)
{
    int index = ((b_tree_array[node_index])->num_keys) - 1;

    if ((b_tree_array[node_index])->leaf == true)
    {
        while ((index >= 0) && (((b_tree_array[node_index])->keys)[index] > key))
        {
            ((b_tree_array[node_index])->keys)[index + 1] = ((b_tree_array[node_index])->keys)[index];
            ((b_tree_array[node_index])->values)[index + 1] = ((b_tree_array[node_index])->values)[index];
            index--;
        }

        ((b_tree_array[node_index])->keys)[index + 1] = key;
        ((b_tree_array[node_index])->values)[index + 1] = key_data;
        (b_tree_array[node_index])->num_keys += 1;
    }

    else
    {
        while ((index >= 0) && (((b_tree_array[node_index])->keys)[index]) > key)
        {
            index--;
        }

        if ((b_tree_array[(b_tree_array[node_index])->children[index + 1]])->num_keys == ((2 * degree) - 1))
        {
            split_child(index + 1, node_index, (((b_tree_array[node_index])->children)[index + 1]));

            if ((((b_tree_array[node_index])->keys)[index + 1]) < key)
            {
                index++;
            }
        }

        insert_non_full(key, ((b_tree_array[node_index])->children)[index + 1], key_data);
    }
}

int findKey(int node_index, int key)
{
    int index = 0;

    while ((index < (b_tree_array[node_index])->num_keys) && (((b_tree_array[node_index])->keys)[index] < key))
    {
        index++;
    }

    return index;
}

void remove_key(int node_index, int key)
{
    int found_index = findKey(node_index, key);

    if ((found_index < ((b_tree_array[node_index])->num_keys)) && ((((b_tree_array[node_index])->keys)[found_index]) == key))
    {
        if ((b_tree_array[node_index])->leaf)
        {
            remove_from_leaf(node_index, found_index);
        }

        else
        {
            remove_from_non_leaf(node_index, found_index);
        }
    }

    else
    {
        if ((b_tree_array[node_index])->leaf)
        {
            printf("Error: Key Doesn't Exist\n");
            return;
        }

        bool flag = ((found_index == ((b_tree_array[node_index])->num_keys))? true : false);

        if ((b_tree_array[((b_tree_array[node_index])->children[found_index])])->num_keys  < ((b_tree_array[node_index])->t))
        {
            fill(node_index, found_index);
        }

        if (flag && (found_index > ((b_tree_array[node_index])->num_keys)))
        {
            remove_key(((b_tree_array[node_index])->children[found_index - 1]), key);
        }

        else
        {
            remove_key(((b_tree_array[node_index])->children[found_index]), key);
        }
    }
}

void remove_from_leaf(int node_index, int found_index)
{
    //printf("Remove from leaf.\n");
    for (int index = found_index+1; index < ((b_tree_array[node_index])->num_keys); ++index)
    {
        ((b_tree_array[node_index])->keys)[index - 1] = ((b_tree_array[node_index])->keys)[index];
        ((b_tree_array[node_index])->values)[index - 1] = ((b_tree_array[node_index])->values)[index];
    }

    (b_tree_array[node_index])->num_keys -= 1;
}

void remove_from_non_leaf(int node_index, int found_index)
{
    //printf("Remove from non leaf.\n");
    int k = ((b_tree_array[node_index])->keys)[found_index];

    if ((b_tree_array[((b_tree_array[node_index])->children[found_index])])->num_keys >= ((b_tree_array[node_index])->t))
    {
        int pred = get_pred(node_index, found_index);
        struct data pred_data = get_pred_data(node_index, found_index);
        ((b_tree_array[node_index])->keys[found_index]) = pred;
        ((b_tree_array[node_index])->values[found_index]) = pred_data;
        remove_key(((b_tree_array[node_index])->children[found_index]), pred);
    }

    else if ((b_tree_array[((b_tree_array[node_index])->children[found_index + 1])])->num_keys >= ((b_tree_array[node_index])->t))
    {
        int succ = get_succ(node_index, found_index);
        struct data succ_data = get_succ_data(node_index, found_index);
        ((b_tree_array[node_index])->keys[found_index]) = succ;
        ((b_tree_array[node_index])->values[found_index]) = succ_data;
        remove_key(((b_tree_array[node_index])->children[found_index + 1]), succ);
    }

    else
    {
        merge(node_index, found_index);
        remove_key(((b_tree_array[node_index])->children[found_index]), k);
    }
}

int get_pred(int node_index, int found_index)
{
    struct tree_node *current = (b_tree_array[((b_tree_array[node_index])->children[found_index])]);
    while (!(current->leaf))
    {
        current = (b_tree_array[current->children[current->num_keys]]);
    }
    return current->keys[(current->num_keys) - 1];
}

struct data get_pred_data(int node_index, int found_index)
{
    struct tree_node *current = (b_tree_array[((b_tree_array[node_index])->children[found_index])]);
    while (!(current->leaf))
    {
        current = (b_tree_array[current->children[current->num_keys]]);
    }
    return current->values[(current->num_keys) - 1];
}

int get_succ(int node_index, int found_index)
{
    struct tree_node *current = (b_tree_array[((b_tree_array[node_index])->children[found_index + 1])]);
    while (!(current->leaf))
    {
        current = (b_tree_array[current->children[0]]);
    }
    return current->keys[0];
}

struct data get_succ_data(int node_index, int found_index)
{
    struct tree_node *current = (b_tree_array[((b_tree_array[node_index])->children[found_index + 1])]);
    while (!(current->leaf))
    {
        current = (b_tree_array[current->children[0]]);
    }
    return current->values[0];
}

int fill(int node_index, int found_index)
{
    if ((found_index != 0) && ((b_tree_array[((b_tree_array[node_index])->children[found_index - 1])])->num_keys >= ((b_tree_array[node_index])->t)))
    {
        borrow_from_prev(node_index, found_index);
    }

    else if ((found_index != (b_tree_array[node_index])->num_keys) && ((b_tree_array[((b_tree_array[node_index])->children[found_index + 1])])->num_keys >= ((b_tree_array[node_index])->t)))
    {
        borrow_from_next(node_index, found_index);
    }

    else
    {
        if (found_index != (b_tree_array[node_index])->num_keys)
        {
            merge(node_index, found_index);
        }

        else
        {
            merge(node_index, (found_index-1));
        }
    }
}

void borrow_from_prev(int node_index, int found_index)
{
    //printf("Borrow from prev.\n");
    struct tree_node *child = (b_tree_array[((b_tree_array[node_index])->children[found_index])]);
    struct tree_node *sibling = (b_tree_array[((b_tree_array[node_index])->children[found_index - 1])]);

    for (int index = (child->num_keys)-1; index >= 0; --index)
    {
        child->keys[index + 1] = child->keys[index];
        child->values[index + 1] = child->values[index];
    }

    if (!(child->leaf))
    {
        for (int index = child->num_keys; index >= 0; --index)
        {
            child->children[index + 1] = child->children[index];
        }
    }

    child->keys[0] = (b_tree_array[node_index])->keys[found_index - 1];
    child->values[0] = (b_tree_array[node_index])->values[found_index - 1];

    if (!(b_tree_array[node_index]->leaf))
    {
        child->children[0] = sibling->children[sibling->num_keys];
    }

    ((b_tree_array[node_index])->keys)[found_index - 1] = sibling->keys[(sibling->num_keys) - 1];
    ((b_tree_array[node_index])->values)[found_index - 1] = sibling->values[(sibling->num_keys) - 1];

    child->num_keys += 1;
    sibling->num_keys -= 1;
}

void borrow_from_next(int node_index, int found_index)
{
    //printf("Borrow from next.\n");
    struct tree_node *child = (b_tree_array[((b_tree_array[node_index])->children[found_index])]);
    struct tree_node *sibling = (b_tree_array[((b_tree_array[node_index])->children[found_index + 1])]);

    child->keys[(child->num_keys)] = (b_tree_array[node_index])->keys[found_index];
    child->values[(child->num_keys)] = (b_tree_array[node_index])->values[found_index];

    if (!(child->leaf))
    {
        child->children[(child->num_keys) + 1] = sibling->children[0];
    }

    ((b_tree_array[node_index])->keys[found_index]) = sibling->keys[0];
    ((b_tree_array[node_index])->values[found_index]) = sibling->values[0];

    for (int index=1; index<(sibling->num_keys); ++index)
    {
        sibling->keys[index-1] = sibling->keys[index];
        sibling->values[index-1] = sibling->values[index];
    }

    if (!(sibling->leaf))
    {
        for (int index=1; index<=(sibling->num_keys); ++index)
        {
            sibling->children[index - 1] = sibling->children[index];
        }
    }

    child->num_keys += 1;
    sibling->num_keys -= 1;
}

void merge(int node_index, int found_index)
{
    //printf("Merge.\n");
    struct tree_node *child = (b_tree_array[((b_tree_array[node_index])->children[found_index])]);
    struct tree_node *sibling = (b_tree_array[((b_tree_array[node_index])->children[found_index + 1])]);

    child->keys[(b_tree_array[node_index]->t)-1] = (b_tree_array[node_index])->keys[found_index];
    child->values[(b_tree_array[node_index]->t)-1] = (b_tree_array[node_index])->values[found_index];

    for (int index=0; index<(sibling->num_keys); ++index)
    {
        child->keys[index + (b_tree_array[node_index]->t)] = sibling->keys[index];
        child->values[index + (b_tree_array[node_index]->t)] = sibling->values[index];
    }

    if (!(child->leaf))
    {
        for (int index=0; index<=(sibling->num_keys); ++index)
        {
            child->children[index + (b_tree_array[node_index]->t)] = sibling->children[index];
        }
    }

    for (int index = (found_index) + 1; index<(b_tree_array[node_index]->num_keys); ++index)
    {
        b_tree_array[node_index]->keys[index - 1] = b_tree_array[node_index]->keys[index];
        b_tree_array[node_index]->values[index - 1] = b_tree_array[node_index]->values[index];
    }

    for (int index = (found_index + 2); index <= (b_tree_array[node_index]->num_keys); ++index)
    {
        b_tree_array[node_index]->children[index - 1] = b_tree_array[node_index]->children[index];
    }

    child->num_keys += sibling->num_keys + 1;
    (b_tree_array[node_index])->num_keys -= 1;
}
