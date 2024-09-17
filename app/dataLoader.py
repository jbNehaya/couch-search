from typing import Dict
import json

class DataLoader:
    def __init__(self, filepath: str):
        self.filepath = filepath
        self.words_index = {}
        self.page_titles = {}
        self.load_data()

    def load_data(self):
        try:
            with open(self.filepath, 'r') as f:
                data = json.load(f)
                self.words_index = data.get("words_index", {})
                self.page_titles = data.get("page_titles", {})
        except FileNotFoundError:
            self.words_index = {}
            self.page_titles = {}

    def get_words_index(self) -> Dict[str, Dict[str, int]]:
        return self.words_index

    def get_page_titles(self) -> Dict[str, str]:
        return self.page_titles
