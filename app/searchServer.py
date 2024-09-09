from fastapi import FastAPI, HTTPException
from typing import List, Dict
import json

app = FastAPI()

try:
    with open('data.json', 'r') as f:
        data = json.load(f)
except FileNotFoundError:
    data = {"words_index": {}, "page_titles": {}}

WORDS_INDEX = data.get("words_index", {})
PAGE_TITLES = data.get("page_titles", {})

def classify_terms_positive_negative(term: str):
    positive_terms = []
    negative_terms = []
    
    terms = term.split()
    for term in terms:
        if term.startswith('-'):
            negative_terms.append(term[1:])
        else:
            positive_terms.append(term)
    
    return positive_terms, negative_terms

def build_response(results: set): 
    response = []
    for url in results:
        title = PAGE_TITLES.get(url, 'Unknown title')
        response.append({'url': url, 'title': title})
    
    return response

@app.get("/search", response_model=List[Dict[str, str]])
async def search(term: str):
    if not term:
        raise HTTPException(status_code=400, detail="No search term provided")
    
    positive_terms, negative_terms = classify_terms_positive_negative(term)


    results = set()

    for word in positive_terms:
        urls = WORDS_INDEX.get(word, {})        
        results.update(urls.keys())

    for word in negative_terms:
        urls = WORDS_INDEX.get(word, {})        
        results.difference_update(urls.keys())


    response = build_response(results)
    print(f"Response for query '{term}': {response}")
    return response


    