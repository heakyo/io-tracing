#!/usr/bin/env python3


"""Interact with Docker images in Artifactory."""


import argparse
import logging
import os
import sys
import typing as t

import requests


ARTIFACTORY_HOST = os.getenv("ARTIFACTORY_HOST", "pscale.artifactory.cec.lab.emc.com")
ARTIFACTORY_REPO = os.getenv("ARTIFACTORY_REPO", "pscale-docker-local")


def image_tags(image: str) -> t.List[str]:
    """Fetch a list of all tags for a given image from Artifactory."""
    response = requests.get(
        f"https://{ARTIFACTORY_HOST}"
        f"/artifactory/api/docker/{ARTIFACTORY_REPO}/v2/{image}/tags/list",
        timeout=300,
    )
    response.raise_for_status()
    return response.json()["tags"]


def image_versions(image: str, *, sort: bool = True) -> t.List[str]:
    """Fetch a list of all version tags for a given image from Artifactory."""
    versions_ = [
        tag
        for tag in image_tags(image)
        if tag.count(".") > 1 and tag.replace(".", "").isdigit()
    ]
    if sort:
        versions_ = sorted(
            versions_,
            key=lambda tag: tuple(int(x) for x in tag.split(".")),
        )
    return versions_


def latest(args: argparse.Namespace) -> None:
    """Print the tag which represents the latest valid version."""
    print(image_versions(args.image, sort=True)[-1])


def tags(args: argparse.Namespace) -> None:
    """Print all tags published on Artifactory."""
    for tag in image_tags(args.image):
        print(tag)


def versions(args: argparse.Namespace) -> None:
    """Print all tags which are valid versions."""
    for tag in image_versions(args.image):
        print(tag)


COMMANDS = {
    "latest": latest,
    "tags": tags,
    "versions": versions,
}


def cli() -> argparse.ArgumentParser:
    """Create a command parser."""
    command_width = max(len(command) for command in COMMANDS)
    parser = argparse.ArgumentParser(
        description=__doc__
        + "\n\ncommands:\n"
        + "\n".join(
            f"  {command:{command_width}}\t{command_func.__doc__}"
            for command, command_func in COMMANDS.items()
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument(
        "command",
        choices=COMMANDS.keys(),
        help="the command to run",
    )
    parser.add_argument("image", help="the name of the Docker image")
    return parser


def main(argv: t.Optional[t.List[str]] = None) -> int:
    """Execute commands."""
    if argv is None:
        argv = sys.argv[1:]
    parser = cli()
    args = parser.parse_args(argv)
    logging.basicConfig(format="[%(asctime)s] %(levelname)s: %(message)s")
    try:
        COMMANDS[args.command](args)
    except Exception as exc:
        message = str(exc)
        if (
            isinstance(exc, requests.HTTPError)
            and exc.response.status_code == requests.codes.unauthorized
        ):
            message += f"\nIs {ARTIFACTORY_HOST} configured in ~/.netrc?"
        logging.error(message)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
