    map<string, vector<int>> gpaIndex = buildIndex<string>(table.rows, 2);   // column 2 = gpa

    for (auto& pair : gpaIndex) {
        cout << pair.first << " -> rows: ";
        for (int pos : pair.second) {
            cout << pos << " ";
        }
        cout << "\n";
    }