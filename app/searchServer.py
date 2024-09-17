from fastapi import FastAPI, HTTPException
from typing import List, Dict
import json
from dataLoader import DataLoader

app = FastAPI()
loader =  DataLoader('data.json')

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

def build_response(results: set, page_titles: Dict[str, str]): 
    response = []
    for url in results:
        title = page_titles.get(url, 'Unknown title')
        response.append({'url': url, 'title': title})
    
    return response

@app.get("/search", response_model=List[Dict[str, str]])
async def search(term: str):
    if not term:
        raise HTTPException(status_code=400, detail="No search term provided")
    
    positive_terms, negative_terms = classify_terms_positive_negative(term)


    results = set()
    WORDS_INDEX = loader.get_words_index()
    PAGE_TITLES = loader.get_page_titles()
    
    for word in positive_terms:
        urls = WORDS_INDEX.get(word, {})        
        results.update(urls.keys())

    for word in negative_terms:
        urls = WORDS_INDEX.get(word, {})        
        results.difference_update(urls.keys())


    response = build_response(results, PAGE_TITLES)
    print(f"Response for query '{term}': {response}")
    return response


    