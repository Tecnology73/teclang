struct User {
    int id;
    int age;
};

int main(int argc, char **argv) {
    User users[argc];
    users[0].id = 0;

    return sizeof(users) / sizeof(User);
}