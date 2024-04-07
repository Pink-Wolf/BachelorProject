import Style from "@/styles/EntityViewer.css";

import Link from "next/link";
import { CodeViewer, Declaration, Reference } from "./CodeComponents";

export default function MethodViewer(props) {
	const data = props.data;
	if (data == undefined) return <div />

	const is_empty = (x) => { return x == undefined || x.length == 0 }

	return (
		<section>
			<h1 id="Path">
				{`${data.namespace}::${data.owner}::${data.name}`}
			</h1>
			<small>
				<p id="Project">In project <Reference path={data.project}>{data.project}</Reference></p>
				<p id="Header">Defined in header <Reference path={`${data.project}/${data.header}`}>&lt;{data.header}&gt;</Reference></p>
				<p id="Class">Member of <Reference path={`${data.project}/${data.header}/${data.owner}`}>{data.owner}</Reference></p>
			</small>

			<ol id="Declarations" type="1">
				{data.overloads.map((x, i) => {
					return (
						<li key={i + 1}>
							<Declaration>{x.declaration}</Declaration>
							{x.description}
						</li>
					)
				})}
			</ol>

			<section hidden={is_empty(data.warnings)} className="warning" id="Warnings">
				<h2>Warning</h2>
				{data.warnings?.map((x, i) => {
					return <blockquote key={i}>{x}</blockquote>
				})}
			</section>

			<section hidden={is_empty(data.exceptions)} id="Exceptions">
				<h2>Exceptions</h2>
				{data.exceptions?.map((x, i) => {
					return <blockquote key={i}>{x}</blockquote>
				})}
			</section>

			<section hidden={!data.example} id="Example">
				<h2>Example</h2>
				<p>{data.example?.description}</p>
				<CodeViewer>
					{data.example?.code}
				</CodeViewer>
			</section>

			<section hidden={is_empty(data.related)} id="Related">
				<h2>Related</h2>
				{data.related?.map((x, i) => {
					return (
						<p key={i}>
							<Link href={"/api/" + x}>
								<code>{x}</code>
							</Link>
						</p>
					)
				})}
			</section>

		</section>
	)
}