# Linguistic Heritage Engine: Cyrillic Ы

**Status:** scoped repository integration  
**Record:** `data/linguistic-heritage/cyrillic-letter-y.json`  
**Contract:** `linguistic-heritage.letter.v1`

This repository participates in the 13-repository Linguistic Heritage Engine, a shared research effort covering grapheme genealogy, historical phonology, modern articulation, comparative Slavic phonetics, orthographic history and cultural interpretation.

## Repository role

Specification-only consumer: Unicode and text metadata may be referenced in documentation. No kernel, ABI, boot, memory or execution contract changes.

The record is reference data, not a new runtime dependency. This integration does not expand this repository's authority or alter product behavior.

## Evidence policy

- Separate the history of the grapheme Ы from the history of the vowel [ɨ].
- Treat ЪІ / ЪИ as the conventional ligature account, without inventing a single creation date.
- Mark the /i/ versus /ɨ/ phonological analysis as debated.
- Do not equate Russian ы, Polish y and Ukrainian и as identical sounds.
- Keep numerology and symbolism in the interpretive layer, never in the evidence layer.
- Require shelfmark, folio and specialist source for manuscript-specific assertions.
- Distinguish evidence, scholarly interpretation and open hypothesis.

## Validation

Run the standard-library conformance test:

```bash
python -m unittest discover -s tests -p 'test_linguistic_heritage_contract.py' -v
```

This checks the local record's structure and research guardrails; it does not prove historical claims.

## Shared ownership

SAMBU owns the canonical data contract and validator; Luci may inspect metadata; Klipot may display it read-only; Hellduck indexes the relationship. All other repositories keep the integration within their existing domain and introduce no mandatory runtime dependency.

## Sources

- Cavar et al., *High central vowels in Polish, Ukrainian and Russian*, JIPA (2025), [DOI](https://doi.org/10.1017/S0025100325000040).
- *Russian*, JIPA (2015), [Cambridge Core](https://www.cambridge.org/core/journals/journal-of-the-international-phonetic-association/article/russian/55589EC639ADEF1764B5ECD0B76970FA).
- [Cyrillic script](https://www.newworldencyclopedia.org/entry/Cyrillic_script) is a secondary starting point for the conventional ligature account; primary palaeographic evidence remains required for precise dating.
