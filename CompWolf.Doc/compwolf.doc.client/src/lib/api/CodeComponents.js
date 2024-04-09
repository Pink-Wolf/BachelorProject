import Link from "next/link";
import SyntaxHighlighter from "react-syntax-highlighter/dist/esm/default-highlight";
import { xcode } from "react-syntax-highlighter/dist/esm/styles/hljs";
import { getPathTo } from "./Entity";
import { Fragment } from "react";

export function Reference({ path, children }) {
	return (
		<Link href={"/api/" + path}>
			<code>
				{children}
			</code>
		</Link>
	)
}
export async function SimpleReference({ name }) {
	const path = await getPathTo(name)
	return <Reference path={path}>{name}</Reference>
}

export function CodeViewer({ children }) {
	return (
		<SyntaxHighlighter language="cpp" style={xcode}>
			{children}
		</SyntaxHighlighter>
	)
}
export function Declaration({ children }) {
	return (
		<big className="declaration">
			<CodeViewer>
				{children}
			</CodeViewer>
		</big>
	)
}

export function FormattedText({ children }) {
	if (children === undefined) return false
	return (
		<Fragment>
			{
				children
					.split(/\[\[/g)
					.map(s => s.split(/\]\]/, 2))
					.map((s, i) => {
						if (s[1] === undefined) return <Fragment key={i}>{s[0]}</Fragment>;
						return <Fragment key={i}><SimpleReference name={s[0]} />{s[1]}</Fragment>
					})
			}
		</Fragment>
	)
}
