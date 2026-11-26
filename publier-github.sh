#!/usr/bin/env bash
set -euo pipefail
src="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
owner=philippe015
repo="$owner/ed-unofficial-patch"
branch=feature/openxr-onfoot-stability
command -v gh >/dev/null || { echo 'Installer GitHub CLI (gh) et git.'; exit 1; }
command -v git >/dev/null
gh auth status
[[ "$(gh api user --jq .login)" == "$owner" ]] || { echo 'Compte GitHub incorrect.'; exit 1; }
if ! gh repo view "$repo" >/dev/null 2>&1; then
  gh repo fork characterecho-sean/ed-unofficial-patch --clone=false
fi
parent="$(gh api "repos/$repo" --jq '.parent.full_name // ""')"
[[ "$parent" == 'characterecho-sean/ed-unofficial-patch' ]] || { echo 'Le dépôt cible ne correspond pas au fork attendu.'; exit 1; }
if gh api "repos/$repo/branches/$branch" >/dev/null 2>&1; then
  echo 'Branche déjà existante : aucune modification effectuée.'; exit 1
fi
work="$(mktemp -d)"
trap 'rm -rf -- "$work"' EXIT
gh repo clone "$repo" "$work/repo" -- --depth 1
cd "$work/repo"
git switch -c "$branch"
[[ ! -e vr-controllers && ! -e appliquer-integration.py && ! -e .github/workflows/vr-input-core.yml ]] || { echo 'Conflit de fichiers : arrêt.'; exit 1; }
cp -R "$src/vr-controllers" .
cp "$src/appliquer-integration.py" . 
mkdir -p .github/workflows
cp "$src/.github/workflows/vr-input-core.yml" .github/workflows/
git add vr-controllers appliquer-integration.py .github/workflows/vr-input-core.yml
git commit -m 'Add OpenXR on-foot controller mapper v0.9 stability and diagnostics'
git push -u origin "$branch"
printf '\nBranche : https://github.com/%s/tree/%s\n' "$repo" "$branch"
