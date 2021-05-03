QTreeView {
    alternate-background-color: #232629; background-color: #31363b; show-decoration-selected: 0;
}

QTreeView::branch {
    alternate-background-color: #232629; background-color: #31363b;
}

QTreeView::branch:open:has-children:!has-siblings,
QTreeView::branch:open:has-children:has-siblings {
    image: url(stylesheets/down_arrow.png);
}

QTreeView::branch:has-children:!has-siblings:closed,
QTreeView::branch:closed:has-children:has-siblings {
    image: url(stylesheets/right_arrow.png);
}