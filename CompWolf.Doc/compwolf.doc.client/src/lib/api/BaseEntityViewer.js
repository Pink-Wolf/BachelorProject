import Style from "@/styles/EntityViewer.css";

import { CodeViewer, FormattedText, Reference, SimpleReference } from "./CodeComponents";

export default function BaseEntityViewer({ data, top, children }) {
	if (data == undefined) return <p>{`404: Could not get the entity's data.`}</p>

	const is_empty = (x) => { return x == undefined || x.length == 0 }
	const is_owned = (data.owner != undefined)

	return (
		<section>
			<h1 id="Path">
				{`${data.namespace}${is_owned ? `::${data.owner}` : ``}::${data.name}`}
			</h1>
			<small>
				<p id="Project">In project <Reference path={data.project}>{data.project}</Reference></p>
				<p id="Header">Defined in header <Reference path={`${data.project}/${data.header}`}>&lt;{data.header}&gt;</Reference></p>
				<p id="Owner" hidden={!is_owned}>Member of <Reference path={`${data.project}/${data.header}/${data.owner}`}>{data.owner}</Reference></p>
			</small>

			{top}

			<section hidden={is_empty(data.warnings)} className="warning" id="Warnings">
				<h2>Warning</h2>
				{data.warnings?.map((x, i) => {
					return <blockquote key={i}><FormattedText>{x}</FormattedText></blockquote>
				})}
			</section>

			{children}

			<section hidden={!data.example} id="Example">
				<h2>Example</h2>
				<p>
					<FormattedText>{data.example?.description}</FormattedText>
				</p>
				<CodeViewer>
					{data.example?.code}
				</CodeViewer>
			</section>

			<section hidden={is_empty(data.related)} id="Related">
				<h2>Related</h2>
				<ul>
					{data.related?.map((x, i) => {
						return (
							<li key={i}>
								{<SimpleReference name={x} />}
							</li>
						)
					})}
				</ul>
			</section>

		</section>
	)
}