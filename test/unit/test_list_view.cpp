#include <catch.hpp>

#include <list_view.h>

TEST_CASE("ListViews can be constructed")
{
    int data[5] = {};
    ListView<int> listView = makeListView(5, data);

    CHECK(listView.capacity == 5);
    CHECK(listView.count == 0);
    CHECK(listView.elems == data);
}

TEST_CASE("ListViews can have elements added to them")
{
    int data[5] = {};
    ListView<int> listView = makeListView(5, data);

    REQUIRE(listView.count == 0);

    add(listView, 2);

    REQUIRE(listView.count == 1);
    CHECK(listView[0] == 2);

    add(listView, 9);

    REQUIRE(listView.count == 2);
    CHECK(listView[0] == 2);
    CHECK(listView[1] == 9);

    add(listView, 7);

    REQUIRE(listView.count == 3);
    CHECK(listView[0] == 2);
    CHECK(listView[1] == 9);
    CHECK(listView[2] == 7);
}

TEST_CASE("ListViews can be cleared")
{
    int data[5] = {};
    ListView<int> listView = makeListView(5, data);

    add(listView, 2);
    add(listView, 9);
    add(listView, 7);

    REQUIRE(listView.count == 3);

    clear(listView);

    CHECK(listView.count == 0);
}
