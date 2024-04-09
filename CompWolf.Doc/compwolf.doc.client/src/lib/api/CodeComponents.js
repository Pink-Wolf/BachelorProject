import Link from "next/link";
import SyntaxHighlighter from "react-syntax-highlighter/dist/esm/default-highlight";
import { xcode } from "react-syntax-highlighter/dist/esm/styles/hljs";
import { getPathTo } from "./Entity";

export function Reference(props) {
	return (
		<Link href={"/api/" + props.path}>
			<code>
				{props.children}
			</code>
		</Link>
	)
}
export async function SimpleReference({ name }) {
	const path = await getPathTo(name)
	return <Reference path={path}>{name}</Reference>
}

export function CodeViewer(props) {
	return (
		<SyntaxHighlighter language="cpp" style={xcode}>
			{props.children}
		</SyntaxHighlighter>
	)
}
export function Declaration(props) {
	return (
		<big className="declaration">
			<CodeViewer>
				{props.children}
			</CodeViewer>
		</big>
	)
}
